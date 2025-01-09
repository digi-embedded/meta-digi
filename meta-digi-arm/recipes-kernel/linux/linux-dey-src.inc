# Copyright (C) 2018-2021 Digi International

LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=bbea815ee2795b2f4230826c0c6b8814"

LOCALVERSION = "-dey"
SRCBRANCH = "v5.4/dey-3.2/maint"
SRCREV = "${AUTOREV}"

# Select internal or Github Linux repo
LINUX_URI_STASH = "${DIGI_MTK_GIT}/linux/linux.git;protocol=ssh"
LINUX_URI_GITHUB = "${DIGI_GITHUB_GIT}/linux.git;protocol=https"
LINUX_GIT_URI ?= "${@oe.utils.conditional('DIGI_INTERNAL_GIT', '1' , '${LINUX_URI_STASH}', '${LINUX_URI_GITHUB}', d)}"

SRC_URI = "${LINUX_GIT_URI};branch=${SRCBRANCH}"

S = "${WORKDIR}/git"
