# Copyright (C) 2020-2021 Digi International.

# Digi: include patches/files from this layer
FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

WESTON_SRC ?= "git://source.codeaurora.org/external/imx/weston-imx.git;protocol=https"
SRC_URI = " \
    ${WESTON_SRC};branch=${SRCBRANCH} \
    file://weston.png \
    file://weston.desktop \
    file://xwayland.weston-start \
    file://0001-weston-launch-Provide-a-default-version-that-doesn-t.patch \
"
SRCREV = "26da63a46b926c8301d8c271f6869c893cc35afa"

EXTRA_OEMESON_remove = "-Dbackend-rdp=false"
PACKAGECONFIG_append = " rdp"
PACKAGECONFIG[rdp] = "-Dbackend-rdp=true,-Dbackend-rdp=false,freerdp"

# Digi: fix ccimx6 suspend/resume issue
SRC_URI_append_ccimx6 = " file://0001-libweston-g2d-renderer-try-re-adjusting-fb-if-the-FB.patch"
