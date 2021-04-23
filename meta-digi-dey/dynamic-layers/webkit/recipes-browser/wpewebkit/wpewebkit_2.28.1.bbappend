# Copyright 2020, Digi International Inc.

FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

SRC_URI_append_ccimx8x = " file://0001-Use-imxvideoconvert_g2d-plugin-in-gstreamer-pipeline.patch"

# The Qt WPE plugin depends on libgbm, which isn't available for i.MX6
# platforms. It also pulls in some fairly large Qt dependencies, so remove it.
PACKAGECONFIG_remove = "qtwpe"

# We can't build the WebKit with fb images, so force wayland as a required
# distro feature.
inherit features_check

REQUIRED_DISTRO_FEATURES = "wayland"
