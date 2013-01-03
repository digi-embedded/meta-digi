#
# Copyright (C) 2012 Digi International.
#
DESCRIPTION = "Debug applications packagegroup for DEL image"
LICENSE = "GPLv2"
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
	packagegroup-del-debug \
	packagegroup-del-debug-dbg \
	packagegroup-del-debug-dev \
    "

RDEPENDS_${PN} = "\
    packagegroup-core-tools-debug \
    memwatch \
    fbtest \
    ${MACHINE_ESSENTIAL_EXTRA_RDEPENDS}"

RRECOMMENDS_${PN} = "\
    ${MACHINE_ESSENTIAL_EXTRA_RRECOMMENDS}"
