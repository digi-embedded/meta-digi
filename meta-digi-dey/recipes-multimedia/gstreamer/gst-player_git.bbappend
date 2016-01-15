# Copyright (C) 2016 Digi International

FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

SRC_URI_append = " file://0001-gstplayer-force-use-glimagesink.patch"
