# Copyright (C) 2013 Digi International

# Internal repo branch
SRCBRANCH = "master"

SRCREV_external = "8b22aa710c72a5daeb6f79852f1a4fb08abf37d4"
SRCREV_internal = "80a0de8cc2bdbe0d2762e6da6defe89210f6db9c"
SRCREV = "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${SRCREV_internal}', '${SRCREV_external}', d)}"

SRC_URI_external = "${DIGI_GITHUB_GIT}/yocto-uboot.git;protocol=git;nobranch=1"
SRC_URI_internal = "${DIGI_GIT}u-boot-denx.git;protocol=git;nobranch=1"
SRC_URI = "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${SRC_URI_internal}', '${SRC_URI_external}', d)}"

UBOOT_NVRAM_LIBPATH = "git/common/digi/cmd_nvram/lib"
