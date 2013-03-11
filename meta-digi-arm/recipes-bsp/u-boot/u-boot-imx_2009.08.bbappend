# Copyright (C) 2012 Digi International

DESCRIPTION = "bootloader for Digi platforms"

PR_append = "+${DISTRO}.r0"

SRCREV_mxs = "57dc7fb843484ff0099f6e5f5a2528a7a2dd1766"
SRCREV_mx5 = "57dc7fb843484ff0099f6e5f5a2528a7a2dd1766"
SRC_URI = "${DIGI_LOG_GIT}u-boot-denx.git;protocol=git;branch=refs/heads/master"

DEPENDS_mxs_ccardimx28js += "elftosb-native imx-bootlets-del"
DEPENDS_mxs_cpx2 += "elftosb-native imx-bootlets-del"

UBOOT_MAKE_TARGET_ccardimx28js = "u-boot-ivt.sb"
UBOOT_MAKE_TARGET_cpx2 = "u-boot-ivt.sb"

BOOTLETSDIR_mxs = "BOOTLETS_DIR=${STAGING_DIR_TARGET}/boot/"
EXTRA_OEMAKE += '${BOOTLETSDIR}'

COMPATIBLE_MACHINE = "(ccardimx28js|ccimx51js|ccimx53js|cpx2)"
