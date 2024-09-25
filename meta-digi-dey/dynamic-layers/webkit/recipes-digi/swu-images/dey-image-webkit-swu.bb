# Copyright (C) 2020, Digi International Inc.

require recipes-digi/swu-images/swu.inc

# Point to the SRC_URI files in the original swu-images directory
FILESEXTRAPATHS:prepend := "${THISDIR}/../../../../recipes-digi/swu-images/files:"

IMG_NAME = "${@get_baseimg_pn(d)}-${GRAPHICAL_BACKEND}"
