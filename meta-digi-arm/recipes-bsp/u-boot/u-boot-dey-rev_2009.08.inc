# Copyright (C) 2013 Digi International

SRCREV_external = ""
SRCREV_internal = "d4f95c9d1a6619841ce07b8ff2b63d7676c6d6cc"
SRCREV = "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${SRCREV_internal}', '${SRCREV_external}', d)}"

SRC_URI_external = "${DIGI_GITHUB_GIT}/yocto-uboot.git;protocol=git"
SRC_URI_internal = "${DIGI_LOG_GIT}u-boot-denx.git;protocol=git"
SRC_URI = "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${SRC_URI_internal}', '${SRC_URI_external}', d)}"

UBOOT_NVRAM_LIBPATH = "git/common/digi/cmd_nvram/lib"
