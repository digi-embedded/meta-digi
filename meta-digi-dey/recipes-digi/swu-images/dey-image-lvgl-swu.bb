# Copyright (C) 2023,2024, Digi International Inc.

require swu.inc

IMG_NAME = "${@get_baseimg_pn(d)}-${GRAPHICAL_BACKEND}"

# Remove GRAPHICAL_BACKEND suffix (-x11) from ccimx6ul image names
IMG_NAME:ccimx6ul = "${@get_baseimg_pn(d)}"
