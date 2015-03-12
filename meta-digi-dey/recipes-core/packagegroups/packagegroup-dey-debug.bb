#
# Copyright (C) 2012 Digi International.
#
SUMMARY = "Debug applications packagegroup for DEY image"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://${COREBASE}/LICENSE;md5=3f40d7994397109285ec7b81fdeb3b58"
PACKAGE_ARCH = "${MACHINE_ARCH}"

inherit packagegroup

RDEPENDS_${PN} = "\
    evtest \
    fbtest \
    i2c-tools \
    memwatch \
    packagegroup-core-tools-debug \
    tcpdump \
"
