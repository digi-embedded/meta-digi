# Copyright (C) 2018, Digi International Inc.

FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

SRC_URI += " \
    file://0001-keep-resolv.conf-rights.patch \
    file://0002-dhclient-Check-if-the-rebind-time-has-expired-when-r.patch \
"
