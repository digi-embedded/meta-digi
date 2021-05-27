# Copyright 2020-2021 Digi International Inc.

# We can't build the WebKit with fb images, so force wayland as a required
# distro feature.
inherit features_check

REQUIRED_DISTRO_FEATURES = "wayland"
