# Copyright (C) 2013 Digi International

SRCBRANCH_external = "v2009.08/master"
SRCBRANCH_internal = "master"
SRCBRANCH = "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${SRCBRANCH_internal}', '${SRCBRANCH_external}', d)}"

SRCREV_external = "8b22aa710c72a5daeb6f79852f1a4fb08abf37d4"
SRCREV_internal = "b23ff7f11a0577037becc20bbcfe61640fe87fb4"
SRCREV = "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${SRCREV_internal}', '${SRCREV_external}', d)}"

SRC_URI_external = "${DIGI_GITHUB_GIT}/yocto-uboot.git;protocol=git;branch=${SRCBRANCH}"
SRC_URI_internal = "${DIGI_GIT}u-boot-denx.git;protocol=git;branch=${SRCBRANCH}"
SRC_URI = "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${SRC_URI_internal}', '${SRC_URI_external}', d)}"

UBOOT_NVRAM_LIBPATH = "git/common/digi/cmd_nvram/lib"
