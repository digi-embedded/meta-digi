# Copyright (C) 2012 Digi International

DESCRIPTION = "bootloader for Digi platforms"

PR_append = "+digi.0"

SRC_URI = "git://log-sln-git.digi.com/u-boot-denx.git;tag=agonzal/yocto"

EXTRA_OEMAKE += 'HOSTSTRIP=true'

UBOOT_MAKE_TARGET = "u-boot.bin"
UBOOT_SUFFIX = "bin"
UBOOT_PADDING = "2"


