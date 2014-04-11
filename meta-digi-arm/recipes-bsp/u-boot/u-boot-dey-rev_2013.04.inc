# Copyright (C) 2013 Digi International

SRCBRANCH_external = "v2013.04/master"
SRCBRANCH_internal = "v2013.04/master"
SRCBRANCH = "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${SRCBRANCH_internal}', '${SRCBRANCH_external}', d)}"

SRCREV_external = "71a9263c710a58c1c97307238507c6e7804101ee"
SRCREV_internal = "8f5e0b45f3f2e70bcec80176d2c8bf199e29ac9b"
SRCREV = "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${SRCREV_internal}', '${SRCREV_external}', d)}"

SRC_URI_external = "${DIGI_GITHUB_GIT}/yocto-uboot.git;protocol=git;branch=${SRCBRANCH}"
SRC_URI_internal = "${DIGI_GIT}u-boot-denx.git;protocol=git;branch=${SRCBRANCH}"
SRC_URI = "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${SRC_URI_internal}', '${SRC_URI_external}', d)}"
