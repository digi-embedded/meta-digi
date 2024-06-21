# Copyright (C) 2016-2024 Digi International Inc.

FILESEXTRAPATHS:prepend:dey := "${THISDIR}/files:"

SRC_URI:append:dey = " \
    file://0001-colors-modify-psplash-colors-to-match-Digi-scheme.patch \
    file://psplash-digi-bar.png \
"

do_configure:prepend:dey() {
	\cp --remove-destination ${WORKDIR}/psplash-digi-bar.png ${S}/base-images/psplash-bar.png
}
