# Copyright (C) 2016-2025, Digi International Inc.

FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

PV = "4.9.2+git${SRCPV}"

SRC_URI:append:ccimx6ul = " \
    file://0001-gstimxv4l2-map-dev-video1-to-dev-fb0.patch \
"

SRCBRANCH = "MM_04.09.02_2410_L6.6.y"
SRCREV = "ef9c1a080e739e6f0be878148d9f4a050dc83bec"

INSANE_SKIP:append:mx6-nxp-bsp = " 32bit-time"
