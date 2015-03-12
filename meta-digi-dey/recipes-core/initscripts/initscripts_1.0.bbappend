# Copyright (C) 2013 Digi International.

FILESEXTRAPATHS_prepend := "${THISDIR}/${BP}:"

SRC_URI += "file://device_table.txt \
	    file://devtmpfs.sh"

do_install_append() {
	install -m 755 ${WORKDIR}/devtmpfs.sh ${D}${sysconfdir}/init.d/devtmpfs.sh
	update-rc.d -r ${D} devtmpfs.sh start 03 S .
	install -m 0755 ${WORKDIR}/device_table.txt ${D}${sysconfdir}/device_table
}
