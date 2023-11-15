# Copyright 2023 Digi International Inc.

#
# Reuse meta-freescale's imx-dsp_2.0.2.bb
#
require recipes-multimedia/imx-dsp/imx-dsp_2.0.2.bb

LIC_FILES_CHKSUM = "file://COPYING;md5=db4762b09b6bda63da103963e6e081de"

SRC_URI[md5sum] = "2b2581a4b24735f4e449a161a334e04d"
SRC_URI[sha256sum] = "11f4e89c0d3c61ac591aa3e00e345d7cc8d0d2627a915253f920cdcf4492a7d5"

COMPATIBLE_MACHINE = "(ccimx93)"
