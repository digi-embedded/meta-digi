#
# Copyright (C) 2012 Digi International.
#
SUMMARY = "Audio packagegroup for DEY image"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/LICENSE;md5=3f40d7994397109285ec7b81fdeb3b58"
PACKAGE_ARCH = "${MACHINE_ARCH}"
ALLOW_EMPTY_${PN} = "1"
PR = "r0"

inherit packagegroup

ALSA_UTILS_PKGS = " \
    alsa-utils-alsactl \
    alsa-utils-alsamixer \
    alsa-utils-amixer \
    alsa-utils-aplay \
    alsa-utils-speakertest \
"

RDEPENDS_${PN} = "\
    alsa-lib \
    alsa-state \
    alsa-states \
    ${ALSA_UTILS_PKGS} \
"
