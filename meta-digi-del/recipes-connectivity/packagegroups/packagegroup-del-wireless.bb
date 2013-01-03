#
# Copyright (C) 2012 Digi International.
#
DESCRIPTION = "Wireless packagegroup for DEL image"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/LICENSE;md5=3f40d7994397109285ec7b81fdeb3b58"
PACKAGE_ARCH = "${MACHINE_ARCH}"
ALLOW_EMPTY = "1"
PR = "r0"

#
# Set by the machine configuration with packages essential for device bootup
#
MACHINE_ESSENTIAL_EXTRA_RDEPENDS ?= ""
MACHINE_ESSENTIAL_EXTRA_RRECOMMENDS ?= ""

PACKAGES = "\
	packagegroup-del-wireless \
	packagegroup-del-wireless-dbg \
	packagegroup-del-wireless-dev \
    "

RDEPENDS_${PN} = "\
	wpa-supplicant \
	wireless-tools \
        crda \
	iw \
	${MACHINE_ESSENTIAL_EXTRA_RDEPENDS}"

RRECOMMENDS_${PN} = "\
    ${MACHINE_ESSENTIAL_EXTRA_RRECOMMENDS}"
