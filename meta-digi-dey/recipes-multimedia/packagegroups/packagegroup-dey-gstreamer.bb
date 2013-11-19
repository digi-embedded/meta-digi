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
    gst-plugins-base-meta \
    gst-plugins-good-meta \
    gst-plugins-ugly-meta \
    gst-plugins-bad-meta \
    ${MACHINE_GSTREAMER_PLUGIN} \
"

ALLOW_EMPTY_${PN} = "1"

PACKAGE_ARCH = "${MACHINE_ARCH}"
