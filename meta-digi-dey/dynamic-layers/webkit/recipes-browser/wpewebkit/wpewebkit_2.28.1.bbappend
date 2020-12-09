# Copyright 2020, Digi International Inc.

FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

SRC_URI_append_ccimx8x = " file://0001-Use-imxvideoconvert_g2d-plugin-in-gstreamer-pipeline.patch"

# We can't build the WebKit with fb images, so force wayland as a required
# distro feature.
inherit distro_features_check

REQUIRED_DISTRO_FEATURES = "wayland"
