#
# Copyright (C) 2020 Digi International.
#
require recipes-core/images/dey-image-graphical.inc

DESCRIPTION = "DEY image with WebKit browser engine support"

GRAPHICAL_CORE = "webkit"

COMPATIBLE_MACHINE = "(ccimx8x|ccimx8m|ccimx6$)"
