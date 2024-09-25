#
# Copyright (C) 2023,2024, Digi International Inc.
#
require dey-image-graphical.inc

DESCRIPTION = "DEY image with LVGL graphical libraries"

GRAPHICAL_CORE = "lvgl"

# On the ccimx6ul, the only supported LVGL backend is fbdev, so there is no
# need for a X11 desktop environment.
IMAGE_FEATURES:remove:ccimx6ul = " x11-base x11-sato "

# Remove GRAPHICAL_BACKEND suffix (-x11) from ccimx6ul image names
DEFAULT_IMAGE_BASENAME:ccimx6ul = "dey-image-${GRAPHICAL_CORE}"
