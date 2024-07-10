# Copyright (C) 2023,2024, Digi International Inc.

#
# Reuse meta-freescale's linux-imx-headers_5.15.bb
#
require recipes-kernel/linux/linux-imx-headers_5.15.bb

SRCBRANCH = "lf-6.1.y"
LOCALVERSION = "-lts-${SRCBRANCH}"
SRCREV = "770c5fe2c1d1529fae21b7043911cd50c6cf087e"

IMX_UAPI_HEADERS:remove = "isl29023.h"

COMPATIBLE_MACHINE = "(ccimx93)"
