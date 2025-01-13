# Copyright (C) 2024,2025, Digi International Inc.
FILESEXTRAPATHS:prepend := "${THISDIR}/${BP}:"

SRC_URI += " \
    file://0001-Restore-wl_shell-to-weston-12.patch \
    file://0002-Revert-libweston-libinput-device-Enable-Set-pointer-.patch \
    file://0003-Revert-g2d-renderer-Support-solid-colour-weston_buff.patch \
"

EXTRA_OEMESON += "-Ddeprecated-wl-shell=true"

# This package is already in RRECOMMENDS, but it doesn't get included in the
# SDK due to it being a soft dependency from a complementary package. Make it a
# hard dependency so it gets included.
# See: poky commit 4705dd264681d908f144dd4d9bf1f6175f68d8b9
RDEPENDS:${PN}-dev += "wayland-protocols"
