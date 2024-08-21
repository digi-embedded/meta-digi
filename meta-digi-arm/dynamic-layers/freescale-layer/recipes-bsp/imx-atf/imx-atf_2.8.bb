# Copyright (C) 2023,2024, Digi International Inc.

#
# Reuse meta-freescale's imx-atf_2.6.bb
#
require recipes-bsp/imx-atf/imx-atf_2.6.bb

SRC_URI = "git://github.com/nxp-imx/imx-atf.git;protocol=https;branch=${SRCBRANCH}"
SRCBRANCH = "lf_v2.8"
# Tag: lf-6.1.55-2.2.0
SRCREV = "08e9d4eef2262c0dd072b4325e8919e06d349e02"

COMPATIBLE_MACHINE = "(ccimx93)"
