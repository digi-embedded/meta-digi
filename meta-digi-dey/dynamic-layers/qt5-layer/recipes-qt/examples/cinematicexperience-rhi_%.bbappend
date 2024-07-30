# Copyright (C) 2023, Digi International Inc.

FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SRC_URI += "file://icon_qt.png"

do_install:append() {
	install -d ${D}${datadir}/icons/hicolor/24x24
	install -m 0644 ${WORKDIR}/icon_qt.png ${D}${datadir}/icons/hicolor/24x24/

	ln -sf qt5-cinematic-experience ${D}${bindir}/cinematic-experience
}
