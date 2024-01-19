# Copyright 2023 Digi International Inc.

#
# Reuse meta-freescale's wayland-protocols_1.25.imx.bb
#
require recipes-graphics/wayland/wayland-protocols_1.25.imx.bb

LIC_FILES_CHKSUM = "file://LICENSE;md5=c7b12b6702da38ca028ace54aae3d484 \
                    file://stable/presentation-time/presentation-time.xml;endline=26;md5=4646cd7d9edc9fa55db941f2d3a7dc53"

SRC_URI = "git://github.com/nxp-imx/wayland-protocols-imx.git;protocol=https;branch=wayland-protocols-imx-1.32"
SRCREV = "7ece577d467f8afb2f5a2f7fff3761a1e0ee9dad"

BBCLASSEXTEND = "native nativesdk"

COMPATIBLE_MACHINE = "(ccimx93)"
