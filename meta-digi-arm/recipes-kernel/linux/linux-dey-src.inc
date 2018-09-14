# Copyright (C) 2018 Digi International

LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=d7810fab7487fb0aad327b76f1be7cd7"

LOCALVERSION = "-dey"
SRCBRANCH = "v4.9/dey-2.4/maint"
SRCBRANCH_ccimx8x = "v4.9/dey-2.4/maint_cc8x"
SRCREV = "${AUTOREV}"

# Select internal or Github Linux repo
LINUX_GIT_URI ?= "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${DIGI_GIT}linux-2.6.git', '${DIGI_GITHUB_GIT}/linux.git', d)}"
SRC_URI = "${LINUX_GIT_URI};branch=${SRCBRANCH}"

S = "${WORKDIR}/git"
