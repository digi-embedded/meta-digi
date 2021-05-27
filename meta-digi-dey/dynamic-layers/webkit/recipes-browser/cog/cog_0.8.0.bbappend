# Copyright 2020-2021 Digi International Inc.

FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

SRC_URI_append = " \
    file://0001-cog-remove-the-platform-parameter-and-hardcode-the-F.patch \
    file://0002-cog-platform-fdo-always-use-fullscreen-mode.patch \
"

EXTRA_OECMAKE += "-DCOG_HOME_URI=http://127.0.0.1/"
