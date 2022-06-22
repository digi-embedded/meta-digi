# Copyright (C) 2020-2022 Digi International, Inc.

FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

SRC_URI:append:imx-nxp-bsp = " \
    file://0001-gpio-uapi-add-userspace-support-for-setting-debounce.patch \
"
