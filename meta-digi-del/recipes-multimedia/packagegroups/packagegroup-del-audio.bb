#
# Copyright (C) 2012 Digi International.
#
SUMMARY = "Audio packagegroup for DEL image"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/LICENSE;md5=3f40d7994397109285ec7b81fdeb3b58"
PACKAGE_ARCH = "${MACHINE_ARCH}"
ALLOW_EMPTY = "1"
PR = "r0"

inherit packagegroup

RDEPENDS_${PN} = "\
    alsa-lib \
    alsa-utils \
    alsa-state \
    alsa-states \
    ${MACHINE_ESSENTIAL_EXTRA_RDEPENDS}"


RDEPENDS_${PN}_append_mx5 = " imx-audio"

RRECOMMENDS_${PN} = "\
    ${MACHINE_ESSENTIAL_EXTRA_RRECOMMENDS}"
