# Copyright (C) 2026, Digi International Inc.
FILESEXTRAPATHS:prepend := "${THISDIR}/${BP}:"

SRCREV = "5223a3c86177709d25f86a96622c0829da955a0e"

SRC_URI += " \
    file://0001-libweston-g2d-renderer-try-re-adjusting-fb-if-the-FB.patch \
"
