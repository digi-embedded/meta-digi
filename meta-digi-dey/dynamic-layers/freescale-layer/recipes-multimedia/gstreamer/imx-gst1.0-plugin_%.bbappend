# Copyright (C) 2016-2024, Digi International Inc.

FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

SRC_URI:append:ccimx6ul = " \
    file://0001-gstimxv4l2-map-dev-video1-to-dev-fb0.patch \
    file://0002-imx-gst1.0-plugin-fix-build-using-MUSL-C-library.patch \
"

LIC_FILES_CHKSUM:ccimx9 = "file://LICENSE.txt;md5=fbc093901857fcd118f065f900982c24"
PV:ccimx9 = "4.8.2+git${SRCPV}"
SRCBRANCH:ccimx9 = "MM_04.08.02_2310_L6.1.y"
SRCREV:ccimx9 = "a72df52acfec5f849ec93906e33cb50da01b0b2e"
