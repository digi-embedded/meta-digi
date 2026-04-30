# Copyright (C) 2019-2026, Digi International Inc.

FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

SRC_URI += " \
    file://0001-libsocketcan-Get-and-set-CAN-FD-data-bitrate.patch \
"

