# Copyright (C) 2012-2015 Digi International

require recipes-bsp/u-boot/u-boot.inc

DESCRIPTION = "Bootloader for Digi platforms"
LICENSE = "GPLv2+"
LIC_FILES_CHKSUM = "file://Licenses/README;md5=c7383a594871c03da76b3707929d2919"

DEPENDS += "dtc-native u-boot-mkimage-native"

PROVIDES += "u-boot"

SRCBRANCH = "v2015.04/master"
SRCREV = "${AUTOREV}"

# Select internal or Github U-Boot repo
UBOOT_GIT_URI = "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${DIGI_GIT}u-boot-denx.git', '${DIGI_GITHUB_GIT}/u-boot.git', d)}"

SRC_URI = " \
    ${UBOOT_GIT_URI};branch=${SRCBRANCH} \
    file://boot.txt \
"

LOCALVERSION ?= ""
inherit fsl-u-boot-localversion

EXTRA_OEMAKE_append = " KCFLAGS=-fgnu89-inline"

do_deploy_append() {
	# Boot script for DEY images
	mkimage -T script -n bootscript -C none -d ${WORKDIR}/boot.txt ${DEPLOYDIR}/boot.scr
}

COMPATIBLE_MACHINE = "(ccimx6)"
