# Copyright 2020, Digi International Inc.

FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

SRC_URI_append = " \
    file://0001-platform-add-a-common-EGL-proc-address-loader-with-d.patch \
    file://0002-egl-proc-address.h-add-a-license-header.patch \
    file://0003-cog-remove-the-platform-parameter-and-hardcode-the-F.patch \
    file://0004-cog-platform-fdo-always-use-fullscreen-mode.patch \
"

EXTRA_OECMAKE += "-DCOG_HOME_URI=www.digi.com"
