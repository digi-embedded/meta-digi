#  Copyright (C) 2019 by Digi International Inc.

FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

SRC_URI += "\
    file://0001-system.conf-reduce-default-stop-timeout-to-15-second.patch \
"
