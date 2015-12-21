# Copyright (C) 2015 Digi International.

LIC_FILES_CHKSUM = "file://main.cpp;md5=a3ecdc1d777da347f1bf35dd9966606f"

DEPENDS += "qtmultimedia"

SRCREV = "35d72a2eba7456a2efc5eb8b77afbc00f69ba0ac"

do_install_append() {
	# New version of the example application embed all the files in the
	# binary itself using QRC
	rm -rf ${D}${datadir}/${P}/qml
}

RDEPENDS_${PN} += "qtmultimedia-qmlplugins"
