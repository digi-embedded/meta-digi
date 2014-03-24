# Copyright (C) 2013 Digi International

SRCBRANCH = "v2013.04/master"

SRCREV_external = ""
SRCREV_internal = "c1a7f08a7768b28beb2a26cf0bcd236347b4a9e0"
SRCREV = "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${SRCREV_internal}', '${SRCREV_external}', d)}"

SRC_URI_external = "${DIGI_GITHUB_GIT}/yocto-uboot.git;protocol=git"
SRC_URI_internal = "${DIGI_LOG_GIT}u-boot-denx.git;protocol=git;branch=${SRCBRANCH}"
SRC_URI = "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${SRC_URI_internal}', '${SRC_URI_external}', d)}"
