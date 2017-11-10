# Copyright (C) 2013 Digi International

SRCBRANCH = "v2013.01/dub-2.0/maint"
SRCREV = "bed2e47919bea4a87482fbeb6fd4cf08ba1fd355"

# Select internal or Github U-Boot repo
UBOOT_GIT_URI ?= "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${DIGI_GIT}u-boot-denx.git', '${DIGI_GITHUB_GIT}/u-boot.git', d)}"

SRC_URI = "${UBOOT_GIT_URI};nobranch=1"

UBOOT_NVRAM_LIBPATH = "git/board/digi/common/cmd_nvram/lib"
