# Copyright 2024 Digi International Inc.

#
# Reuse poky's gstreamer1.0-plugins-ugly_1.20.7.bb
#
require recipes-multimedia/gstreamer/gstreamer1.0-plugins-ugly_1.20.7.bb

LIC_FILES_CHKSUM = "file://COPYING;md5=a6f89e2100d9b6cdffcea4f398e37343"

SRC_URI[sha256sum] = "3e31454c98cb2f7f6d2d355eceb933a892fa0f1dc09bc36c9abc930d8e29ca48"

COMPATIBLE_MACHINE = "(ccimx93)"
