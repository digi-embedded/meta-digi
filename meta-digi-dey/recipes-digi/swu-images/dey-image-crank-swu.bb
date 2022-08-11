# Copyright (C) 2022 Digi International Inc.

require swu.inc

# Remove X11 image features
IMAGE_FEATURES:remove:ccimx6ul = "x11-base x11-sato"

IMG_NAME = "${@get_baseimg_pn(d)}-${GRAPHICAL_BACKEND}"
