# Copyright (C) 2018 Digi International

LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=d7810fab7487fb0aad327b76f1be7cd7"

LOCALVERSION = "-dey"
SRCBRANCH = "v4.9/dey-2.4/maint"
SRCBRANCH_ccimx8x = "v4.9/dey-2.4/maint_cc8x"
SRCREV = "a78b3d90543e3b751d02be146eea6ecef98026d5"
SRCREV_ccimx8x = "c5bb360542d53dc8cfceaec5f2d9060e47fdc5bc"

# Select internal or Github Linux repo
LINUX_GIT_URI ?= "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${DIGI_GIT}linux-2.6.git', '${DIGI_GITHUB_GIT}/linux.git', d)}"
SRC_URI = "${LINUX_GIT_URI};nobranch=1"

S = "${WORKDIR}/git"
