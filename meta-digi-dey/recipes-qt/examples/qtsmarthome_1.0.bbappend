# Copyright (C) 2017, Digi International Inc.

FILESEXTRAPATHS_prepend := "${THISDIR}/${BP}:"

SRC_URI += "file://0001-qtsmarthome-fix-runtime-warning.patch"

RDEPENDS_${PN} += "qtsvg-plugins"
