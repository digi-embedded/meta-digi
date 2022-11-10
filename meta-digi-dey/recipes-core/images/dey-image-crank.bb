# Copyright (C) 2022, Digi International Inc.

require dey-image-graphical.inc

DESCRIPTION = "DEY image including Crank Storyboard engine and demo packages"

GRAPHICAL_CORE = "crank"

# Remove X11 image features
IMAGE_FEATURES:remove:ccimx6ul = "x11-base x11-sato"

# Reduce the rootfs size to fit in the internal storage
IMAGE_FEATURES:remove:ccmp15 = "dey-gstreamer package-management"
