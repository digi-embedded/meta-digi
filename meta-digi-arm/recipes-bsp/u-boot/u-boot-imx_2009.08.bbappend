# Copyright (C) 2012 Digi International

DESCRIPTION = "bootloader for Digi platforms"

PR_append = "+del.r0"

SRC_URI = "${DIGI_LOG_GIT}u-boot-denx.git;tag=agonzal/yocto"

EXTRA_OEMAKE += 'HOSTSTRIP=true'

UBOOT_MAKE_TARGET = "u-boot.bin"
UBOOT_SUFFIX = "bin"
UBOOT_PADDING = "2"

COMPATIBLE_MACHINE = "(ccardxmx28js|ccxmx51js|ccxmx53js)"
