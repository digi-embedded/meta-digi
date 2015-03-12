# Copyright (C) 2013 Digi International.

SUMMARY = "Example GPIO kernel module."
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=12f884d2ae1ff87c09e5b7ccc2c4ca7e"

inherit module

PV = "2.1"

SRC_URI = "\
    file://COPYING \
    file://gpio.c \
    file://gpio.h \
    file://Makefile \
"

S = "${WORKDIR}"
