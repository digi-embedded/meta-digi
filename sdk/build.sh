#!/bin/bash
#===============================================================================
#
#  build.sh
#
#  Copyright (C) 2013 by Digi International Inc.
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
#     DY_BUILD_TCHAIN:   Build toolchains for DEY images
#     DY_BUILD_VARIANTS: Build all platform variants
#     DY_DISTRO:         Distribution name (the default is 'dey')
#     DY_PLATFORMS:      Platforms to build
#     DY_REVISION:       Revision of the manifest repository (for 'repo init')
#     DY_RM_WORK:        Remove the package working folders to save disk space.
#     DY_TARGET:         Target image (the default is 'dey-image-qt')
#     DY_USE_MIRROR:     Use internal Digi mirror to download packages
#
#===============================================================================

set -e

SCRIPTNAME="$(basename ${0})"
SCRIPTPATH="$(cd $(dirname ${0}) && pwd)"

MANIFEST_URL="ssh://git@stash.digi.com/dey/digi-yocto-sdk-manifest.git"

DIGI_PREMIRROR_CFG="
# Use internal mirror
SOURCE_MIRROR_URL = \"http://log-sln-jenkins.digi.com/yocto/downloads/\"
INHERIT += \"own-mirrors\"
BB_GENERATE_MIRROR_TARBALLS = \"1\"
"

RM_WORK_CFG="
INHERIT += \"rm_work\"
# Exclude rm_work for some key packages (for debugging purposes)
RM_WORK_EXCLUDE += \"dey-image-qt linux-dey qtbase u-boot-dey\"
"

ZIP_INSTALLER_CFG="
DEY_IMAGE_INSTALLER = \"1\"
"

X11_REMOVAL_CFG="
DISTRO_FEATURES_remove = \"x11\"
"

REPO="$(which repo)"

error() {
	printf "${1}"
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
	if echo ${JOB_NAME} | grep -qs 'dey.*release'; then
		cp -r tmp/deploy/* ${1}/
	else
		cp -r tmp/deploy/images ${1}/
		if [ "${DY_BUILD_TCHAIN}" = "true" ]; then
			if [ -d tmp/deploy/sdk ]; then
				cp -r tmp/deploy/sdk ${1}/
			fi
		fi
	fi

	# Images directory post-processing
	#  - Jenkins artifact archiver does not copy symlinks, so remove them
	#    beforehand to avoid ending up with several duplicates of the same
	#    files.
	#  - Remove 'README_-_DO_NOT_DELETE_FILES_IN_THIS_DIRECTORY.txt' files
	#  - Create MD5SUMS file
	find ${1} -type l -delete
	find ${1} -type f -name 'README_-_DO_NOT_DELETE*' -delete
	find ${1} -type f -not -name MD5SUMS -print0 | xargs -r -0 md5sum | sed -e "s,${1}/,,g" | sort -k2,2 > ${1}/MD5SUMS
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
		packagegroup-dey-debug \
		packagegroup-dey-examples \
		packagegroup-dey-gstreamer \
		packagegroup-dey-network \
		packagegroup-dey-qt \
		packagegroup-dey-wireless \
	"
	bitbake -k -c cleansstate ${PURGE_PKGS} >/dev/null 2>&1 || true
}

#
# For a given image recipe print the SWU recipe (if it exists)
#
#  $1: image recipe
#
swu_recipe_name() {
	if [ -n "$(find ${YOCTO_INST_DIR}/sources/meta-digi -type f -name "${1}-swu.bb")" ]; then
		printf "${1}-swu"
	fi
}

# Sanity check (Jenkins environment)
[ -z "${DY_PLATFORMS}" ]      && error "DY_PLATFORMS not specified"
[ -z "${DY_REVISION}" ]       && error "DY_REVISION not specified"
[ -z "${DY_RM_WORK}" ]        && error "DY_RM_WORK not specified"
[ -z "${DY_USE_MIRROR}" ]     && error "DY_USE_MIRROR not specified"
[ -z "${WORKSPACE}" ]         && error "WORKSPACE not specified"

# Set default settings if Jenkins does not do it
[ -z "${DY_DISTRO}" ] && DY_DISTRO="dey"

# If DY_BUILD_TCHAIN is unset, set it for release jobs
[ -z "${DY_BUILD_TCHAIN}" ] && [[ "${JOB_NAME}" =~ dey-.*-release ]] && DY_BUILD_TCHAIN="true"

# If DY_FB_IMAGE is unset, set it depending on the job name
if [ -z "${DY_FB_IMAGE}" ] && echo ${JOB_NAME} | grep -qs 'dey.*fb'; then
	DY_FB_IMAGE="true"
fi

# If DY_MFG_IMAGE is unset, set it depending on the job name
if [ -z "${DY_MFG_IMAGE}" ] && echo ${JOB_NAME} | grep -qs 'dey.*mfg'; then
	DY_MFG_IMAGE="true"
fi

if [ -n "${DY_MACHINES_LAYER}" ]; then
	MACHINES_LAYER="-m ${DY_MACHINES_LAYER}"
fi

# Per-platform data
while read _pl _var _tgt; do
	# DY_BUILD_VARIANTS comes from Jenkins environment:
	#   'false':     don't build variants (only the default)
	#   <empty>:     build all the variants supported by the platform
	#   'var1 var2': build the ones specified in the variable
	if [ -n "${DY_BUILD_VARIANTS}" ]; then
		if echo "${DY_BUILD_VARIANTS}" | grep -qs "false"; then
			_var="DONTBUILDVARIANTS"
		else
			_var="${DY_BUILD_VARIANTS}"
		fi
	fi
	[ -n "${DY_TARGET}" ] && _tgt="${DY_TARGET}" || true
	eval "${_pl}_var=\"${_var}\""
	eval "${_pl}_tgt=\"${_tgt}\""
done<<-_EOF_
	ccardimx28js      - e w wb web web1   dey-image-qt
	ccimx6sbc         DONTBUILDVARIANTS   dey-image-qt
	ccimx6ulsbc       DONTBUILDVARIANTS   dey-image-qt
	ccimx6ulstarter   DONTBUILDVARIANTS   core-image-base
	ccimx6ulsom       DONTBUILDVARIANTS   dey-image-mft-module-min
	ccimx6ulrftest    DONTBUILDVARIANTS   dey-image-mft-module-rf
_EOF_

YOCTO_IMGS_DIR="${WORKSPACE}/images"
YOCTO_INST_DIR="${WORKSPACE}/digi-yocto-sdk.$(echo ${DY_REVISION} | tr '/' '_')"
YOCTO_PROJ_DIR="${WORKSPACE}/projects"

CPUS="$(grep -c processor /proc/cpuinfo)"
[ ${CPUS} -gt 1 ] && MAKE_JOBS="-j${CPUS}"

printf "\n[INFO] Build Yocto \"${DY_REVISION}\" for \"${DY_PLATFORMS}\" (cpus=${CPUS})\n\n"

# Install/Update Digi's Yocto SDK
mkdir -p ${YOCTO_INST_DIR}
if pushd ${YOCTO_INST_DIR}; then
	# Use git ls-remote to check the revision type
	if [ "${DY_REVISION}" != "master" ]; then
		if git ls-remote --tags --exit-code "${MANIFEST_URL}" "${DY_REVISION}"; then
			printf "[INFO] Using tag \"${DY_REVISION}\"\n"
			repo_revision="-b refs/tags/${DY_REVISION}"
		elif git ls-remote --heads --exit-code "${MANIFEST_URL}" "${DY_REVISION}"; then
			printf "[INFO] Using branch \"${DY_REVISION}\"\n"
			repo_revision="-b ${DY_REVISION}"
		else
			error "Revision \"${DY_REVISION}\" not found"
		fi
	fi
	# If it is a manufacturing job, specify the manufacturing manifest name
	if [ "${DY_MFG_IMAGE}" = "true" ]; then
		mfg_xml="-m manufacturing.xml"
	fi
	yes "" 2>/dev/null | ${REPO} init --no-repo-verify -u ${MANIFEST_URL} ${repo_revision} ${mfg_xml}
	${REPO} forall -p -c 'git remote prune $(git remote)'
	time ${REPO} sync -d ${MAKE_JOBS}
	popd
fi

# Create projects and build
rm -rf ${YOCTO_IMGS_DIR} ${YOCTO_PROJ_DIR}
for platform in ${DY_PLATFORMS}; do
	eval platform_variants="\${${platform}_var}"
	eval platform_targets="\${${platform}_tgt}"
	for variant in ${platform_variants}; do
		_this_prj_dir="${YOCTO_PROJ_DIR}/${platform}"
		_this_img_dir="${YOCTO_IMGS_DIR}/${platform}"
		if [ "${variant}" != "DONTBUILDVARIANTS" ]; then
			_this_prj_dir="${YOCTO_PROJ_DIR}/${platform}_${variant}"
			_this_img_dir="${YOCTO_IMGS_DIR}/${platform}_${variant}"
			_this_var_arg="-v ${variant}"
			[ "${variant}" = "-" ] && _this_var_arg="-v \\"
		fi
		mkdir -p ${_this_img_dir} ${_this_prj_dir}
		if pushd ${_this_prj_dir}; then
			# Configure and build the project in a sub-shell to avoid
			# mixing environments between different platform's projects
			(
				export TEMPLATECONF="${TEMPLATECONF:+${TEMPLATECONF}/${platform}}"
				MKP_PAGER="" . ${YOCTO_INST_DIR}/mkproject.sh -p ${platform} ${MACHINES_LAYER} ${_this_var_arg} <<< "y"
				# Set a common DL_DIR and SSTATE_DIR for all platforms
				sed -i  -e "/^#DL_DIR ?=/cDL_DIR ?= \"${YOCTO_PROJ_DIR}/downloads\"" \
					-e "/^#SSTATE_DIR ?=/cSSTATE_DIR ?= \"${YOCTO_PROJ_DIR}/sstate-cache\"" \
					conf/local.conf
				# Set the DISTRO and remove 'meta-digi-dey' layer if distro is not DEY based
				sed -i -e "/^DISTRO ?=/cDISTRO ?= \"${DY_DISTRO}\"" conf/local.conf
				if ! echo "${DY_DISTRO}" | grep -qs "dey"; then
					sed -i -e '/meta-digi-dey/d' conf/bblayers.conf
				fi
				if [ "${DY_USE_MIRROR}" = "true" ]; then
					sed -i -e "s,^#DIGI_INTERNAL_GIT,DIGI_INTERNAL_GIT,g" conf/local.conf
					printf "${DIGI_PREMIRROR_CFG}" >> conf/local.conf
				fi
				if [ "${DY_RM_WORK}" = "true" ]; then
					printf "${RM_WORK_CFG}" >> conf/local.conf
				fi
				printf "${ZIP_INSTALLER_CFG}" >> conf/local.conf
				# Remove 'x11' distro feature if building framebuffer images
				if [ "${DY_FB_IMAGE}" = "true" ]; then
					printf "${X11_REMOVAL_CFG}" >> conf/local.conf
				fi
				# Append extra configuration macros if provided from build environment
				if [ -n "${DY_EXTRA_LOCAL_CONF}" ]; then
					printf "%s\n" "${DY_EXTRA_LOCAL_CONF}" >> conf/local.conf
				fi
				# Add the manufacturing layer to bblayers.conf file if it is a manufacturing job
				if [ "${DY_MFG_IMAGE}" = "true" ]; then
					sed -i -e "/meta-digi-dey/a\  ${YOCTO_INST_DIR}/sources/meta-digi-mfg \\\\" conf/bblayers.conf
				fi
				for target in ${platform_targets}; do
					printf "\n[INFO] Building the ${target} target.\n"
					time bitbake ${target} $(swu_recipe_name ${target})
					# Build the toolchain for DEY images
					if [ "${DY_BUILD_TCHAIN}" = "true" ] && echo "${target}" | grep -qs '^\(core\|dey\)-image-[^-]\+$'; then
						printf "\n[INFO] Building the toolchain for ${target}.\n"
						time bitbake -c populate_sdk ${target}
					fi
				done
				purge_sstate
			)
			copy_images ${_this_img_dir}
			popd
		fi
	done
done
