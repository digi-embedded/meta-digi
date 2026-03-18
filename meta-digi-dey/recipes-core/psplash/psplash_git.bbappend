# Copyright (C) 2016-2026, Digi International Inc.

FILESEXTRAPATHS:prepend:dey := "${THISDIR}/files:"
SPLASH_LOGO_PNG ?= "${WORKDIR}/digi_logo.png"

SPLASH_IMAGES = "file://logo.png;outsuffix=default"

SRC_URI:append:dey = " \
    file://0001-colors-modify-psplash-colors-to-match-Digi-scheme.patch \
    file://psplash-digi-bar.png \
"

do_configure:prepend:dey() {
	\cp --remove-destination ${WORKDIR}/psplash-digi-bar.png ${S}/base-images/psplash-bar.png
	if [ "${SPLASH_LOGO_PNG}" != "${WORKDIR}/digi_logo.png" ]; then
		\cp --remove-destination "${SPLASH_LOGO_PNG}" "${WORKDIR}/logo.png"
	fi
}