# Copyright (C) 2016-2020 Digi International Inc.

FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

SRC_URI += " \
    file://0001-gstimxv4l2-map-dev-video1-to-dev-fb0.patch \
    file://0002-imx-gst1.0-plugin-fix-build-using-MUSL-C-library.patch \
"
SRC_URI_append_ccimx6 = " file://0002-imx_2d_device_g2d-define-a-G2D_AMPHION_INTERLACED-ma.patch"
