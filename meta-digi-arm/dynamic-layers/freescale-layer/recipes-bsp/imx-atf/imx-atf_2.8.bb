# Copyright 2023 Digi International Inc.

#
# Reuse meta-freescale's imx-atf_2.6.bb
#
require recipes-bsp/imx-atf/imx-atf_2.6.bb

SRC_URI = "git://github.com/nxp-imx/imx-atf.git;protocol=https;branch=${SRCBRANCH}"
SRCBRANCH = "lf_v2.8"
# Tag: lf-6.1.36-2.1.0
SRCREV = "1a3beeab6484343a4bd0ee08e947d142db4a5ae6"

COMPATIBLE_MACHINE = "(ccimx93)"
