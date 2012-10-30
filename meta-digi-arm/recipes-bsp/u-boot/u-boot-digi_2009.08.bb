# Copyright (C) 2011 Freescale Semiconductor
# Copyright (C) 2012 Digi International
# Released under the MIT license (see COPYING.MIT for the terms)

DESCRIPTION = "bootloader for Digi platforms"
require recipes-bsp/u-boot/u-boot.inc

PROVIDES += "u-boot"

LICENSE = "GPLv2+"
LIC_FILES_CHKSUM = "file://COPYING;md5=4c6cde5df68eff615d36789dc18edd3b"

DEPENDS_mxs += "elftosb-native"

PR = "r8"

# Revision for MX28 based platforms.
SRCREV_mxs = "DUB-4.1.2"

# Revision for MX51 and MX53 platforms
# [DIGI] This is currently the agonzal/yocto branch
# [DIGI] until DIGI-UBOOT-93 is fixed.
SRCREV_mx5 = "f684e0259c4a9f63476a3694d9f0f5a6d21b1943"

SRC_URI = "git://log-sln-cvs.digi.com/data/vcs/git/u-boot-denx.git"

UBOOT_MACHINE_ccxmx53js = "ccxmx53js_config"
UBOOT_MACHINE_ccxmx51js = "ccxmx51js_config"
UBOOT_MACHINE_ccardxmx28js = "ccardxmx28js_config"

UBOOT_MAKE_TARGET = "u-boot.bin"
UBOOT_SUFFIX = "bin"
UBOOT_PADDING = "2"

S = "${WORKDIR}/git"
EXTRA_OEMAKE += 'HOSTSTRIP=true'

PACKAGE_ARCH = "${MACHINE_ARCH}"

# [DIGI] GNU gold linker is an alternative to GNU linker.
do_compile_prepend() {
	if [ "${@base_contains('DISTRO_FEATURES', 'ld-is-gold', 'ld-is-gold', '', d)}" = "ld-is-gold" ] ; then
		sed -i 's/$(CROSS_COMPILE)ld/$(CROSS_COMPILE)ld.bfd/g' config.mk
	fi
}

COMPATIBLE_MACHINE = "(mxs|mx5)"

