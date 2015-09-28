#!/bin/bash
#===============================================================================
#
#  build-github.sh
#
#  Copyright (C) 2015 by Digi International Inc.
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
#     DY_PLATFORMS: Platforms to build
#     DY_REVISION:  Revision of the manifest repository (for 'repo init')
#     DY_TARGET:    Target image (the default is 'dey-image-minimal')
#
#===============================================================================

set -e

AVAILABLE_PLATFORMS="ccardimx28js ccimx6sbc"

MANIFEST_URL="https://github.com/digidotcom/dey-manifest.git"

RM_WORK_CFG="
INHERIT += \"rm_work\"
# Exclude rm_work for some key packages (for debugging purposes)
RM_WORK_EXCLUDE += \"dey-image-graphical dey-image-minimal linux-dey u-boot-dey\"
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

# Sanity checks (Jenkins environment)
[ -z "${DY_REVISION}" ] && error "DY_REVISION not specified"
[ -z "${WORKSPACE}" ] && error "WORKSPACE not specified"

# Set default values if not provided by Jenkins
[ -z "${DY_PLATFORMS}" ] && DY_PLATFORMS="$(echo ${AVAILABLE_PLATFORMS})"
[ -z "${DY_TARGET}" ] && DY_TARGET="dey-image-graphical"

YOCTO_IMGS_DIR="${WORKSPACE}/images"
YOCTO_INST_DIR="${WORKSPACE}/dey.$(echo ${DY_REVISION} | tr '/' '_')"
YOCTO_PROJ_DIR="${WORKSPACE}/projects"

CPUS="$(grep -c processor /proc/cpuinfo)"
[ ${CPUS} -gt 1 ] && MAKE_JOBS="-j${CPUS}"

printf "\n[INFO] Build Yocto \"${DY_REVISION}\" for \"${DY_PLATFORMS}\" (cpus=${CPUS})\n\n"

# Install DEY
rm -rf ${YOCTO_INST_DIR} && mkdir -p ${YOCTO_INST_DIR}
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
	yes "" 2>/dev/null | ${REPO} init --no-repo-verify -u ${MANIFEST_URL} ${repo_revision}
	time ${REPO} sync ${MAKE_JOBS}
	popd
fi

# Create projects and build
rm -rf ${YOCTO_IMGS_DIR} ${YOCTO_PROJ_DIR}
for platform in ${DY_PLATFORMS}; do
	_this_prj_dir="${YOCTO_PROJ_DIR}/${platform}"
	_this_img_dir="${YOCTO_IMGS_DIR}/${platform}"
	mkdir -p ${_this_img_dir} ${_this_prj_dir}
	if pushd ${_this_prj_dir}; then
		# Configure and build the project in a sub-shell to avoid
		# mixing environments between different platform's projects
		(
			export TEMPLATECONF="${TEMPLATECONF:+${TEMPLATECONF}/${platform}}"
			MKP_PAGER="" . ${YOCTO_INST_DIR}/mkproject.sh -p ${platform} <<< "y"
			# Set a common DL_DIR and SSTATE_DIR for all platforms
			sed -i  -e "/^#DL_DIR ?=/cDL_DIR ?= \"${YOCTO_PROJ_DIR}/downloads\"" \
				-e "/^#SSTATE_DIR ?=/cSSTATE_DIR ?= \"${YOCTO_PROJ_DIR}/sstate-cache\"" \
				conf/local.conf
			printf "${RM_WORK_CFG}" >> conf/local.conf
			# Remove 'x11' distro feature if building minimal images
			if echo "${DY_TARGET}" | grep -qs "dey-image-minimal"; then
				printf "${X11_REMOVAL_CFG}" >> conf/local.conf
			fi
			for target in ${DY_TARGET}; do
				printf "\n[INFO] Building the $target target.\n"
				time bitbake ${target}
			done
			purge_sstate
		)
		copy_images ${_this_img_dir}
		popd
	fi
done
