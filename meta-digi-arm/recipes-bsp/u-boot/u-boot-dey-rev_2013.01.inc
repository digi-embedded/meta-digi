# Copyright (C) 2013 Digi International

SRCREV_external = ""
SRCREV_internal = "c0fadb69aeab54ad1afb9db1f9ba0701ab8d6807"
SRCREV = "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${SRCREV_internal}', '${SRCREV_external}', d)}"

SRC_URI_external = "${DIGI_GITHUB_GIT}/yocto-uboot.git;protocol=git"
SRC_URI_internal = "${DIGI_GIT}u-boot-denx.git;protocol=git"
SRC_URI = "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${SRC_URI_internal}', '${SRC_URI_external}', d)}"

UBOOT_NVRAM_LIBPATH = "git/board/digi/common/cmd_nvram/lib"
