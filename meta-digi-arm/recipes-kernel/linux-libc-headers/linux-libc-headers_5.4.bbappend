# Copyright (C) 2020 Digi International, Inc.

FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

SRC_URI:append = " \
    file://0001-gpio-uapi-add-userspace-support-for-setting-debounce.patch \
"

