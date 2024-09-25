# Copyright (C) 2024, Digi International Inc.

FILESEXTRAPATHS:prepend := "${THISDIR}/${BP}:"

SRC_URI:append:class-native = " file://0001-x86-work-around-old-GCC-versions-pre-9.0-having-brok.patch"
