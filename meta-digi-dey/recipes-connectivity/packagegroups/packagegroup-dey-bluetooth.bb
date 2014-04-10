#
# Copyright (C) 2012 Digi International.
#
SUMMARY = "Bluetooth packagegroup for DEY image"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/LICENSE;md5=3f40d7994397109285ec7b81fdeb3b58"
PACKAGE_ARCH = "${MACHINE_ARCH}"

PR = "r0"

inherit packagegroup

RDEPENDS_${PN} = "\
	btfilter \
	bluez4 \
	bluez4-testtools \
"
