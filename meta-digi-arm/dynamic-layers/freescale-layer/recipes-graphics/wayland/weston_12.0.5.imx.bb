# Copyright (C) 2025, Digi International Inc.

#
# Reuse meta-freescale's weston_12.0.4.imx.bb
#
require recipes-graphics/wayland/weston_12.0.4.imx.bb

#
# We need to get the rest of SRC_URI artifacts from meta-freescale, so
# "abuse" COREBASE to get the path to "meta-freescale"
#
FILESEXTRAPATHS:prepend := "${COREBASE}/../meta-freescale/recipes-graphics/wayland/weston:"

SRCBRANCH = "weston-imx-12.0.5"
SRCREV = "fce3595b96eab0b2b432ceae070a65db7d16d866"
