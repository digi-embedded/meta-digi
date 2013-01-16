# Copyright (C) 2012 Digi International

DESCRIPTION = "bootloader for Digi platforms"

PR_append = "+del.r0"

SRC_URI = "${DIGI_LOG_GIT}u-boot-denx.git;tag=hpalacio/del-6.x"

DEPENDS_ccardxmx28js += "imx-bootlets-del"

UBOOT_MAKE_TARGET_ccardxmx28js = "u-boot-ivt.sb"

BOOTLETSDIR_mxs = "BOOTLETS_DIR=${STAGING_DIR_TARGET}/boot/"
EXTRA_OEMAKE += '${BOOTLETSDIR}'

COMPATIBLE_MACHINE = "(ccardxmx28js|ccxmx51js|ccxmx53js)"
