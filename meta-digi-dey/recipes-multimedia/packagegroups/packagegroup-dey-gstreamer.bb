#
# Copyright (C) 2012 Digi International.
#
SUMMARY = "Gstreamer framework packagegroup for DEY image"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/LICENSE;md5=3f40d7994397109285ec7b81fdeb3b58"

PR = "r0"

inherit packagegroup

MACHINE_GSTREAMER_PLUGIN ?= ""

RDEPENDS_${PN} = " \
    gst-meta-audio \
    gst-meta-video \
    ${MACHINE_FIRMWARE} \
    ${MACHINE_GSTREAMER_PLUGIN} \
    gst-fsl-plugin-gplay \
"

ALLOW_EMPTY_${PN} = "1"

PACKAGE_ARCH = "${MACHINE_ARCH}"
