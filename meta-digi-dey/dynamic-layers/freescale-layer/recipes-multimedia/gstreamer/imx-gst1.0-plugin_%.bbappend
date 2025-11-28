# Copyright (C) 2016-2025, Digi International Inc.

FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

SRC_URI:append:ccimx6ul = " \
    file://0001-gstimxv4l2-map-dev-video1-to-dev-fb0.patch \
"

INSANE_SKIP:append:mx6-nxp-bsp = " 32bit-time"
