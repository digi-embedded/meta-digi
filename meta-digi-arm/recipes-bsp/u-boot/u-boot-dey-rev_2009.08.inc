# Copyright (C) 2013 Digi International

# Internal repo branch
SRCBRANCH = "v2009.08/dub-1.6/maint"

SRCREV_external = ""
SRCREV_internal = "57e304b3cb7d6e2024038a2b90a40f18d7e07bc6"
SRCREV = "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${SRCREV_internal}', '${SRCREV_external}', d)}"

SRC_URI_external = "${DIGI_GITHUB_GIT}/yocto-uboot.git;protocol=git;nobranch=1"
SRC_URI_internal = "${DIGI_GIT}u-boot-denx.git;protocol=git;nobranch=1"
SRC_URI = "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${SRC_URI_internal}', '${SRC_URI_external}', d)}"

UBOOT_NVRAM_LIBPATH = "git/common/digi/cmd_nvram/lib"
