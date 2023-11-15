# Copyright 2023 Digi International Inc.

#
# Reuse meta-freescale's linux-imx-headers_5.15.bb
#
require recipes-kernel/linux/linux-imx-headers_5.15.bb

SRCBRANCH = "lf-6.1.y"
LOCALVERSION = "-6.1.36-2.1.0"
SRCREV = "04b05c5527e9af8d81254638c307df07dc9a5dd3"

IMX_UAPI_HEADERS:remove = "isl29023.h"

COMPATIBLE_MACHINE = "(ccimx93)"
