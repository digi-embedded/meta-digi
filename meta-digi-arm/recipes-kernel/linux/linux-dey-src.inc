# Copyright (C) 2018-2021 Digi International

LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=bbea815ee2795b2f4230826c0c6b8814"

LOCALVERSION = "-dey"
SRCBRANCH = "v5.4/dey-3.2/maint"
SRCREV = "43e12e2c1baaaf2532f453cbb9f83c5d4aee2b96"

# Select internal or Github Linux repo
LINUX_GIT_URI ?= "${@oe.utils.conditional('DIGI_INTERNAL_GIT', '1' , '${DIGI_GIT}linux-2.6.git', '${DIGI_GITHUB_GIT}/linux.git;protocol=https', d)}"
SRC_URI = "${LINUX_GIT_URI};nobranch=1"

S = "${WORKDIR}/git"
