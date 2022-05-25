# Copyright 2019, Digi International Inc.

FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

SRCREV = "df01f01354d7c44a07370ae27a3d20b52255830b"

SRC_URI += " \
    file://0001-libsocketcan-Get-and-set-CAN-FD-data-bitrate.patch \
"

