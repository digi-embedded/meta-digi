# Copyright (C) 2013-2023 Digi International

SRCBRANCH = "v2013.01/dub-2.0/maint"
SRCREV = "c922ea153a4495514e41099aa003746ea0592db3"

# Select internal or Github U-Boot repo
UBOOT_URI_STASH = "${DIGI_MTK_GIT}/uboot/u-boot-denx.git;protocol=ssh"
UBOOT_URI_GITHUB = "${DIGI_GITHUB_GIT}/u-boot.git;protocol=https"
UBOOT_GIT_URI ?= "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${UBOOT_URI_STASH}', '${UBOOT_URI_GITHUB}', d)}"

SRC_URI = "${UBOOT_GIT_URI};nobranch=1"

UBOOT_NVRAM_LIBPATH = "git/board/digi/common/cmd_nvram/lib"
