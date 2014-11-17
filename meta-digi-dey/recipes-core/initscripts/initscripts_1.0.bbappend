# Copyright (C) 2013 Digi International.

FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}-${PV}:"

SRC_URI += "file://device_table.txt"

do_install_append() {
	install -m 0755 ${WORKDIR}/device_table.txt ${D}${sysconfdir}/device_table
}
