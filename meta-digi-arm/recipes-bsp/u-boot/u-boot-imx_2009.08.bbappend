# Copyright (C) 2012 Digi International

DESCRIPTION = "bootloader for Digi platforms"

PR_append = "+del.r0"

SRC_URI = "${DIGI_LOG_GIT}u-boot-denx.git;tag=hpalacio/del-6.x"

EXTRA_OEMAKE += 'HOSTSTRIP=true'

UBOOT_MAKE_TARGET = "u-boot.bin"
UBOOT_SUFFIX = "bin"
UBOOT_PADDING = "2"

UBOOT_MACHINE_ccxmx51js = "ccxmx51js_config"
UBOOT_MACHINE_ccxmx53js = "ccxmx53js_config"
UBOOT_MACHINE_ccardxmx28js = "ccardxmx28js_config"

COMPATIBLE_MACHINE = "(ccardxmx28js|ccxmx51js|ccxmx53js)"
