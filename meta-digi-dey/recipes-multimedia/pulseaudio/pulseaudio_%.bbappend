# Copyright (C) 2017 Digi International

FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

SRC_URI_append_ccimx6ulsbc = " file://0001-pulseaudio-keep-headphones-volume-in-platforms-witho.patch"

PACKAGE_ARCH = "${MACHINE_ARCH}"
