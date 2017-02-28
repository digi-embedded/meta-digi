# Copyright (C) 2015-2017, Digi International Inc.

FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

SRC_URI_append = " file://qt5.sh"

PACKAGECONFIG_append = " accessibility examples fontconfig sql-sqlite"
PACKAGECONFIG_append_ccimx6 = " icu"
PACKAGECONFIG_append_ccimx6ul = " linuxfb"

do_install_append() {
	install -d ${D}${sysconfdir}/profile.d
	install -m 0755 ${WORKDIR}/qt5.sh ${D}${sysconfdir}/profile.d/
}

PACKAGE_ARCH = "${MACHINE_ARCH}"
