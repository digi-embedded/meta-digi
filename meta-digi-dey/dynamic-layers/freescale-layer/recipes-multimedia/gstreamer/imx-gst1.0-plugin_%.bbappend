# Copyright (C) 2016-2024, Digi International Inc.

FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

SRC_URI:append:ccimx6ul = " \
    file://0001-gstimxv4l2-map-dev-video1-to-dev-fb0.patch \
    file://0002-imx-gst1.0-plugin-fix-build-using-MUSL-C-library.patch \
"
