#
# Copyright (C) 2020-2024, Digi International Inc.
#
require recipes-core/images/dey-image-graphical.inc

DESCRIPTION = "DEY image with WebKit browser engine support"

GRAPHICAL_CORE = "webkit"

IMAGE_INSTALL:append:ccmp25 = " packagegroup-x-linux-ai"

COMPATIBLE_MACHINE = "(ccimx6$|ccimx8m|ccimx8x|ccimx93|ccmp15|ccmp2)"
