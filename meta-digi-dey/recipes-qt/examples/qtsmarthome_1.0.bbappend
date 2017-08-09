# Copyright (C) 2017, Digi International Inc.

FILESEXTRAPATHS_prepend := "${THISDIR}/${BP}:"

SRC_URI += " \
    file://qtsmarthome.desktop \
    file://qtsmarthome.png \
    file://0001-qtsmarthome-fix-runtime-warning.patch \
"

do_install_append() {
	install -d ${D}${datadir}/applications ${D}${datadir}/pixmaps
	install -m 0644 ${WORKDIR}/qtsmarthome.desktop ${D}${datadir}/applications
	install -m 0644 ${WORKDIR}/qtsmarthome.png ${D}${datadir}/pixmaps
}

RDEPENDS_${PN} += "qtsvg-plugins"
