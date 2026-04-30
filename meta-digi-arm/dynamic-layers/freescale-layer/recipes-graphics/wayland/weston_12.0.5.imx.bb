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
SRCREV = "a29bbd0f65e68b9beda47a94144bd9b2801c42cb"
