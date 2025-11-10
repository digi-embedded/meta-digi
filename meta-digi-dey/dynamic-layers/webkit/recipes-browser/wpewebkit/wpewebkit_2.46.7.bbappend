# Copyright (C) 2025, Digi International Inc.
FILESEXTRAPATHS:prepend := "${THISDIR}/${BP}:"

SRC_URI:append = " \
    file://0001-UIProcess-WebProcessPool-always-swap-process-when-us.patch \
"
