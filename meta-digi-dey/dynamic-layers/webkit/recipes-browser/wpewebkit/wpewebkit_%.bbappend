# Copyright 2020-2022 Digi International Inc.

# We can't build the WebKit with fb images, so force wayland as a required
# distro feature.
inherit features_check

REQUIRED_DISTRO_FEATURES = "wayland"

# Limit number of parallel threads make can run to avoid a ninja build issue
PARALLEL_MAKE = "-j ${@oe.utils.cpu_count(at_most=16)}"
