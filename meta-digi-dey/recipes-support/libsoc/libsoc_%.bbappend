# Copyright (C) 2017-2023 Digi International Inc.

FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

SRCREV = "545b354968a9d03008e1e86e14c58e3f8423a20c"

# The meta-openembedded recipe version is 0.8.2 because there have been no new
# releases/tags on this repo since 2016.
# This bbappend overrides the SRC_URI to use a more updated version of the code
# that doesn't correspond to the package latest tagged version.
SRC_URI:append = " \
    file://0001-gpio-pwm-add-delay-to-allow-udev-rules-to-complete.patch \
"

PACKAGECONFIG = "python"
