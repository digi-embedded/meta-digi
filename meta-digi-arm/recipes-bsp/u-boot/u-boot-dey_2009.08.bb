# Copyright (C) 2012 Digi International

DESCRIPTION = "Bootloader for Digi platforms"
require recipes-bsp/u-boot/u-boot.inc
include u-boot-dey-rev_${PV}.inc

PROVIDES += "u-boot"

LICENSE = "GPLv2+"
LIC_FILES_CHKSUM = "file://COPYING;md5=4c6cde5df68eff615d36789dc18edd3b"

SRC_URI_append_mxs = " file://boot-sd.txt"

S = "${WORKDIR}/git"

DEPENDS_mxs += "elftosb-native imx-bootlets-dey u-boot-mkimage-native"

EXTRA_OEMAKE += 'HOSTSTRIP=true'
EXTRA_OEMAKE_append_mxs = ' BOOTLETS_DIR=${STAGING_DIR_TARGET}/boot'

do_deploy_append_mxs() {
	mkimage -T script -n bootscript -C none -d ${WORKDIR}/boot-sd.txt ${DEPLOYDIR}/boot-sd.scr
}

PACKAGE_ARCH = "${MACHINE_ARCH}"
COMPATIBLE_MACHINE = "(mxs|mx5)"
