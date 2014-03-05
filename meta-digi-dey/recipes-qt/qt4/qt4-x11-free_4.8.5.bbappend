# Copyright (C) 2014 Digi International.

FILESEXTRAPATHS_prepend := "${THISDIR}/qt4-${PV}:"

SRC_URI_append_ccimx6adpt = " file://0001-i.MX6-force-egl-visual-ID-33.patch"
