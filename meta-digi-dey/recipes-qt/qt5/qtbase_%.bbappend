# Copyright (C) 2015 Digi International

FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

SRC_URI_append = " file://qt5.sh"

PACKAGECONFIG_append = " accessibility examples icu sql-sqlite"

do_install_append() {
	install -d ${D}${sysconfdir}/profile.d
	install -m 0755 ${WORKDIR}/qt5.sh ${D}${sysconfdir}/profile.d/
}
