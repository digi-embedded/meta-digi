# Copyright (C) 2018-2020 Digi International

LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=bbea815ee2795b2f4230826c0c6b8814"

LOCALVERSION = "-dey"
SRCBRANCH = "v5.4.24/master"
SRCBRANCH_ccimx6 = "v4.9/dey-2.6/maint"
SRCREV = "${AUTOREV}"

# Select internal or Github Linux repo
LINUX_GIT_URI ?= "${@oe.utils.conditional('DIGI_INTERNAL_GIT', '1' , '${DIGI_GIT}linux-2.6.git', '${DIGI_GITHUB_GIT}/linux.git;protocol=https', d)}"
SRC_URI = "${LINUX_GIT_URI};branch=${SRCBRANCH}"

S = "${WORKDIR}/git"
