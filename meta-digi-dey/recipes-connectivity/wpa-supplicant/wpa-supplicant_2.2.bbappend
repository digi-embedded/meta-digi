# Copyright (C) 2013 Digi International.

FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

PACKAGECONFIG ?= "openssl"

SRC_URI += " \
    file://0001-events-Reduce-verbosity-of-scan-events.patch \
"
