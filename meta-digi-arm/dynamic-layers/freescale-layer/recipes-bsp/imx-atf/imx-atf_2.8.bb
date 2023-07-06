# Copyright 2023 Digi International Inc.

#
# Reuse meta-freescale's imx-atf_2.6.bb
#
require recipes-bsp/imx-atf/imx-atf_2.6.bb

SRC_URI = "git://github.com/nxp-imx/imx-atf.git;protocol=https;branch=${SRCBRANCH}"
SRCBRANCH = "lf_v2.8"
SRCREV = "99195a23d3aef485fb8f10939583b1bdef18881c"

COMPATIBLE_MACHINE = "(ccimx93)"
