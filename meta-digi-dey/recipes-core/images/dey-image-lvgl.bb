#
# Copyright (C) 2023,2024 Digi International.
#
require dey-image-graphical.inc

DESCRIPTION = "DEY image with LVGL graphical libraries"

GRAPHICAL_CORE = "lvgl"

# On the ccimx6ul, the only supported LVGL backend is fbdev, so there is no
# need for a X11 desktop environment.
IMAGE_FEATURES:remove:ccimx6ul = " x11-base x11-sato "
