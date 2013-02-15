# Copyright (C) 2012 Digi International

DESCRIPTION = "bootloader for Digi platforms"

PR_append = "+del.r0"

SRCREV_mxs = "${AUTOREV}"
SRCREV_mx5 = "${AUTOREV}"
SRC_URI = "${DIGI_LOG_GIT}u-boot-denx.git;protocol=git;branch=refs/heads/master"

DEPENDS_mxs_ccardimx28js += "elftosb-native imx-bootlets-del"

UBOOT_MAKE_TARGET_ccardimx28js = "u-boot-ivt.sb"

BOOTLETSDIR_mxs = "BOOTLETS_DIR=${STAGING_DIR_TARGET}/boot/"
EXTRA_OEMAKE += '${BOOTLETSDIR}'

COMPATIBLE_MACHINE = "(ccardimx28js|ccimx51js|ccimx53js)"
