#!/bin/bash
#===============================================================================
#
#  build-github.sh
#
#  Copyright (C) 2015-2024 by Digi International Inc.
#  All rights reserved.
#
#  This program is free software; you can redistribute it and/or modify it
#  under the terms of the GNU General Public License version 2 as published by
#  the Free Software Foundation.
#
#
#  !Description: Yocto autobuild script from Jenkins.
#
#  Parameters set by Jenkins:
#     DY_BUILD_RELEASE:  Build for release mode
#     DY_BUILD_TCHAIN:   Build toolchains for DEY images
#     DY_PLATFORMS:      Platforms to build
#     DY_REVISION:       Revision of the manifest repository (for 'repo init')
#     DY_TARGET:         Target image (the default is platform dependent)
#
#===============================================================================

set -e

MANIFEST_URL="https://github.com/digi-embedded/dey-manifest.git"

RM_WORK_CFG="
INHERIT += \"rm_work\"
# Exclude rm_work for some key packages (for debugging purposes)
RM_WORK_EXCLUDE += \"dey-image-qt dey-image-webkit linux-dey qtbase u-boot-dey\"
"

ZIP_INSTALLER_CFG="
DEY_IMAGE_INSTALLER = \"1\"
"

SOURCE_DATE_EPOCH="${SOURCE_DATE_EPOCH:-$(date +%s)}"
BUILD_TIMESTAMP="
SOURCE_DATE_EPOCH = \"${SOURCE_DATE_EPOCH}\"
REPRODUCIBLE_TIMESTAMP_ROOTFS = \"${SOURCE_DATE_EPOCH}\"
"

REPO="$(which repo)"

error() {
	printf "%s" "${1}"
	exit 1
}

#
# Copy buildresults (images, licenses, packages)
#
#  $1: destination directoy
#
copy_images() {
	# Copy individual packages only for 'release' builds, not for 'daily'.
	# For 'daily' builds just copy the firmware images (the buildserver
	# cannot afford such amount of disk space)
	if [ "${DY_BUILD_RELEASE}" = "true" ]; then
		cp -r tmp/deploy/* "${1}"/
	else
		cp -r tmp/deploy/images "${1}"/
		if [ "${DY_BUILD_TCHAIN}" = "true" ]; then
			if [ -d tmp/deploy/sdk ]; then
				cp -r tmp/deploy/sdk "${1}"/
			fi
		fi
	fi

	# Images directory post-processing
	#  - Jenkins artifact archiver does not copy symlinks, so remove them
	#    beforehand to avoid ending up with several duplicates of the same
	#    files.
	#  - Remove 'README_-_DO_NOT_DELETE_FILES_IN_THIS_DIRECTORY.txt' files
	#  - Create MD5SUMS file
	find "${1}" -type l -delete
	find "${1}" -type f -name 'README_-_DO_NOT_DELETE*' -delete
	find "${1}" -type f -not -name MD5SUMS -print0 | xargs -r -0 md5sum | sed -e "s,${1}/,,g" | sort -k2,2 > "${1}"/MD5SUMS
}

#
# Pre-fetch all the source packages (with a retries mechanism)
#
fetch_all() {
	local FETCH_LOG="fetch.log"
	for _ in $(seq 1 3); do
		bitbake --runall=fetch "${1}" 2>&1 | tee "${FETCH_LOG}"
		grep -qs 'Summary.*ERROR' "${FETCH_LOG}" || break
	done
	rm -f "${FETCH_LOG}"
}

#
# In the buildserver we share the state-cache for all the different platforms
# we build in a jenkins job. This may cause problems with some packages that
# have different runtime dependences depending on the platform.
#
# Purge then the state cache of those problematic packages between platform
# builds.
#
purge_sstate() {
	local PURGE_PKGS=" \
		packagegroup-dey-audio \
		packagegroup-dey-bluetooth \
		packagegroup-dey-core \
		packagegroup-dey-crank \
		packagegroup-dey-debug \
		packagegroup-dey-examples \
		packagegroup-dey-gstreamer \
		packagegroup-dey-lvgl \
		packagegroup-dey-network \
		packagegroup-dey-qt \
		packagegroup-dey-webkit \
		packagegroup-dey-wireless \
	"
	bitbake -k -c cleansstate "${PURGE_PKGS}" >/dev/null 2>&1 || true
}

#
# For a given image recipe print the SWU recipe (if it exists)
#
#  $1: image recipe
#
swu_recipe_name() {
	if [ -n "$(find "${YOCTO_INST_DIR}"/sources/meta-digi -type f -name "${1}-swu.bb")" ]; then
		printf "%s-swu" "${1}"
	fi
}

# Sanity check (Jenkins environment)
[ -z "${DY_REVISION}" ]       && error "DY_REVISION not specified"
[ -z "${WORKSPACE}" ]         && error "WORKSPACE not specified"

# Set default settings if Jenkins does not do it
[ -z "${DY_BUILD_RELEASE}" ] && [[ "${JOB_NAME}" =~ dey-.*-release ]] && DY_BUILD_RELEASE="true"

# If DY_BUILD_TCHAIN is unset, set it for release jobs
[ -z "${DY_BUILD_TCHAIN}" ] && [ "${DY_BUILD_RELEASE}" = "true" ] && DY_BUILD_TCHAIN="true"

# Per-platform data
while read -r _pl _tgt; do
	AVAILABLE_PLATFORMS="${AVAILABLE_PLATFORMS:+${AVAILABLE_PLATFORMS} }${_pl}"
	# shellcheck disable=SC2015
	[ -n "${DY_TARGET}" ] && _tgt="${DY_TARGET}" || true
	# Dashes are not allowed in variables so let's substitute them on
	# the fly with underscores.
	eval "${_pl//-/_}_tgt=\"${_tgt//,/ }\""
done<<-_EOF_
	ccimx8mm-dvk         dey-image-qt
	ccimx8mn-dvk         dey-image-qt
	ccimx8x-sbc-pro      dey-image-qt
	ccimx6qpsbc          dey-image-qt
	ccimx6sbc            dey-image-qt
	ccimx6ulsbc          dey-image-qt
	ccimx6ulstarter      core-image-base
	ccmp15-dvk           dey-image-webkit
	ccmp13-dvk           core-image-base
	ccmp25-dvk           dey-image-qt
	ccimx91-dvk          core-image-base
	ccimx93-dvk          dey-image-qt
_EOF_

# Set default values if not provided by Jenkins
DY_PLATFORMS="${DY_PLATFORMS:-${AVAILABLE_PLATFORMS}}"

YOCTO_IMGS_DIR="${WORKSPACE}/images"
YOCTO_INST_DIR="${WORKSPACE}/digi-yocto-sdk-github.$(echo "${DY_REVISION}" | tr '/' '_')"
YOCTO_DOWNLOAD_DIR="${DY_DOWNLOADS:-${WORKSPACE}}/downloads"
YOCTO_PROJ_DIR="${WORKSPACE}/projects"

# If CPUS is unset, set it with the machine cpus
if [ -z "${CPUS}" ]; then
	CPUS="$(grep -c processor /proc/cpuinfo)"
fi
[ "${CPUS}" -gt 1 ] && MAKE_JOBS="-j${CPUS}"

printf "\n[INFO] Build Yocto \"%s\" for \"%s\" (cpus=%s)\n\n" "${DY_REVISION}" "${DY_PLATFORMS}" "${CPUS}"

# Install/Update Digi's Yocto SDK
mkdir -p "${YOCTO_INST_DIR}"
if pushd "${YOCTO_INST_DIR}"; then
	# Use git ls-remote to check the revision type
	if [ "${DY_REVISION}" != "master" ]; then
		if git ls-remote --tags --exit-code "${MANIFEST_URL}" "${DY_REVISION}"; then
			printf "[INFO] Using tag \"%s\"\n" "${DY_REVISION}"
			repo_revision="-b refs/tags/${DY_REVISION}"
		elif git ls-remote --heads --exit-code "${MANIFEST_URL}" "${DY_REVISION}"; then
			printf "[INFO] Using branch \"%s\"\n" "${DY_REVISION}"
			repo_revision="-b ${DY_REVISION}"
		else
			error "Revision \"${DY_REVISION}\" not found"
		fi
	fi
	# shellcheck disable=SC2086
	yes "" 2>/dev/null | ${REPO} init --no-repo-verify -u ${MANIFEST_URL} ${repo_revision}
	${REPO} --no-pager forall -j4 -p -c 'git clean -fdx'
	# shellcheck disable=SC2016
	${REPO} --no-pager forall -j4 -p -c 'git remote prune $(git remote)' || true
	# shellcheck disable=SC2086
	time ${REPO} sync -d ${MAKE_JOBS}
	popd
fi

# Clean downloads directory
if [ "${DY_RM_DOWNLOADS}" = "true" ]; then
	printf "\n[INFO] Removing the downloads folder.\n"
	rm -rf "${YOCTO_DOWNLOAD_DIR}"
fi

# Clean images and projects folders
rm -rf "${YOCTO_IMGS_DIR}" "${YOCTO_PROJ_DIR}"

# Create projects and build
for platform in ${DY_PLATFORMS}; do
	# The variable <platform>_tgt got its dashes converted to
	# underscores, so we must convert also the ones in ${platform}.
	eval "platform_targets=\"\${${platform//-/_}_tgt}\""
	_this_prj_dir="${YOCTO_PROJ_DIR}/${platform}"
	_this_img_dir="${YOCTO_IMGS_DIR}/${platform}"
	mkdir -p "${_this_img_dir}" "${_this_prj_dir}"
	if pushd "${_this_prj_dir}"; then
		# Configure and build the project in a sub-shell to avoid
		# mixing environments between different platform's projects
		(
			export TEMPLATECONF="${TEMPLATECONF:+${TEMPLATECONF}/${platform}}"
			# shellcheck disable=SC1091,SC2086
			MKP_PAGER="" . ${YOCTO_INST_DIR}/mkproject.sh -p "${platform}" <<< "y"
			# Set a common DL_DIR and SSTATE_DIR for all platforms
			sed -i  -e "/^#DL_DIR ?=/cDL_DIR ?= \"${YOCTO_DOWNLOAD_DIR}\"" \
				-e "/^#SSTATE_DIR ?=/cSSTATE_DIR ?= \"${YOCTO_PROJ_DIR}/sstate-cache\"" \
				conf/local.conf
			{
				printf "%s" "${RM_WORK_CFG}"
				printf "%s" "${ZIP_INSTALLER_CFG}"
				printf "%s" "${BUILD_TIMESTAMP}"
			} >> conf/local.conf
			for target in ${platform_targets:?}; do
				printf "\n[INFO] Building the %s target.\n" "${target}"
				time fetch_all "${target}"
				# shellcheck disable=SC2046
				time bitbake "${target}" $(swu_recipe_name "${target}")
				# Build the toolchain for DEY images
				if [ "${DY_BUILD_TCHAIN}" = "true" ] && echo "${target}" | grep -qs '^\(core\|dey\)-image-[^-]\+$'; then
					printf "\n[INFO] Building the toolchain for %s.\n" "${target}"
					time bitbake -c populate_sdk "${target}"
				fi
			done
			purge_sstate
		)
		copy_images "${_this_img_dir}"
		popd
	fi
done
