# Copyright (C) 2012 Digi International

DESCRIPTION = "Bootloader for Digi platforms"
require recipes-bsp/u-boot/u-boot.inc
include u-boot-dey-rev_${PV}.inc

PROVIDES += "u-boot"

LICENSE = "GPLv2+"
LIC_FILES_CHKSUM = "file://COPYING;md5=1707d6db1d42237583f50183a5651ecb"

PR = "r0"

S = "${WORKDIR}/git"

UBOOT_MAKE_TARGET = "u-boot.sb"
UBOOT_SUFFIX = "sb"
UBOOT_IMAGE = 'u-boot-${MACHINE}${@base_conditional( "UBOOT_CONFIG_VARIANT", "", "-", "-${UBOOT_CONFIG_VARIANT}-", d )}${PV}-${PR}.${UBOOT_SUFFIX}'
UBOOT_BINARY = "u-boot.${UBOOT_SUFFIX}"
UBOOT_SYMLINK = 'u-boot-${MACHINE}.${UBOOT_SUFFIX}'

DEPENDS_mxs += "elftosb-native"

PACKAGE_ARCH = "${MACHINE_ARCH}"
COMPATIBLE_MACHINE = "(ccardimx28js|cpx2|wr21)"
