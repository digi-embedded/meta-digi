# Copyright (C) 2016-2017 Digi International Inc.

FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

SRC_URI_append = " file://0002-Adjust-to-another-change-in-the-user-page-API.patch"

KERNEL_MODULE_AUTOLOAD = "cryptodev"
