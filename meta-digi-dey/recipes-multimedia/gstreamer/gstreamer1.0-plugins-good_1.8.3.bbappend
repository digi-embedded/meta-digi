# Copyright 2015-2017, Digi International Inc.

FILESEXTRAPATHS_prepend := "${THISDIR}/${BP}:"

SRC_URI_append = " \
    file://0001-Need-to-check-if-pa-stream-is-still-valid.patch \
    file://0002-Fix-aacpase-error-tolerance-issue.patch \
    file://0003-ximageutil-shouldn-t-implement-transform-if-don-t-su.patch \
"

PACKAGECONFIG_append = " vpx"
