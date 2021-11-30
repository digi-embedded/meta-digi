# Copyright (C) 2018 Digi International

LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=d7810fab7487fb0aad327b76f1be7cd7"

LOCALVERSION = "-dey"
SRCBRANCH = "v4.9/dey-2.4/maint"
SRCBRANCH_ccimx8x = "v4.9/dey-2.4/maint_cc8x"
SRCREV = "7aa9f75d0550e2e31b2fad44f6e25f2ae841ac5a"
SRCREV_ccimx8x = "7fa09f3341781598b38f79bce8c5646d45b76ca8"

# Select internal or Github Linux repo
LINUX_GIT_URI ?= "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${DIGI_GIT}linux-2.6.git', '${DIGI_GITHUB_GIT}/linux.git', d)}"
SRC_URI = "${LINUX_GIT_URI};nobranch=1"

S = "${WORKDIR}/git"
