# Copyright (C) 2025, Digi International Inc.
FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

# Backport patch to fix build with "lbse" disabled
SRC_URI:append = " \
    file://0001-Build-fix-when-LAYER_BASED_SVG_ENGINE-is-off.patch \
"

SRC_URI:append:ccimx8x = " \
    file://0001-DMABufVideoSinkGStreamer-disable-sink-unconditionall.patch \
"
