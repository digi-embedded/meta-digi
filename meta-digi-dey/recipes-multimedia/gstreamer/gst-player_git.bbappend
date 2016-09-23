# Copyright (C) 2016 Digi International

FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

SRC_URI_append_ccimx6 = " file://0001-gstplayer-force-use-glimagesink.patch"

PACKAGE_ARCH = "${MACHINE_ARCH}"
