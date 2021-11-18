# Copyright (C) 2018, 2019 Digi International

LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=d7810fab7487fb0aad327b76f1be7cd7"

LOCALVERSION = "-dey"
SRCBRANCH = "v4.14/dey-2.6/maint"
SRCBRANCH_ccimx6 = "v4.9/dey-2.6/maint"
SRCREV = "0df575b8594a0d95a18c5d29048e8702737e9b02"
SRCREV_ccimx6 = "70caccf50c27f9ccaa4e6c6ae2428fb27121441d"

# Select internal or Github Linux repo
LINUX_GIT_URI ?= "${@oe.utils.conditional('DIGI_INTERNAL_GIT', '1' , '${DIGI_GIT}linux-2.6.git', '${DIGI_GITHUB_GIT}/linux.git;protocol=https', d)}"
SRC_URI = "${LINUX_GIT_URI};nobranch=1"

S = "${WORKDIR}/git"
