# Copyright 2023 Digi International Inc.

#
# Reuse poky's gstreamer1.0-plugins-ugly_1.20.7.bb
#
require recipes-multimedia/gstreamer/gstreamer1.0-plugins-ugly_1.20.7.bb

LIC_FILES_CHKSUM = "file://COPYING;md5=a6f89e2100d9b6cdffcea4f398e37343"

SRC_URI[sha256sum] = "a644dc981afa2d8d3a913f763ab9523c0620ee4e65a7ec73c7721c29da3c5a0c"

COMPATIBLE_MACHINE = "(ccimx93)"
