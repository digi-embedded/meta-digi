# Copyright (C) 2024, Digi International Inc.

#
# Reuse meta-freescale's imx-atf_2.6.bb
#
require recipes-bsp/imx-atf/imx-atf_2.6.bb

SRC_URI = "git://github.com/nxp-imx/imx-atf.git;protocol=https;branch=${SRCBRANCH}"
SRCBRANCH = "lf_v2.10"
# Tag: lf-6.6.23-2.0.0
SRCREV = "49143a1701d9ccd3239e3f95f3042897ca889ea8"

COMPATIBLE_MACHINE = "(ccimx91)"
