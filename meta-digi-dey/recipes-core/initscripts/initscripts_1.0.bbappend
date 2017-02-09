# Copyright (C) 2013-2017 Digi International.

FILESEXTRAPATHS_prepend := "${THISDIR}/${BP}:"

SRC_URI += "file://device_table.txt \
	    file://devtmpfs.sh"

SRC_URI_append_ccimx6ul = " file://caam_jr0_wakeup.sh"

do_install_append() {
	install -m 755 ${WORKDIR}/devtmpfs.sh ${D}${sysconfdir}/init.d/devtmpfs.sh
	update-rc.d -r ${D} devtmpfs.sh start 03 S .
	install -m 0755 ${WORKDIR}/device_table.txt ${D}${sysconfdir}/device_table
}

do_install_append_ccimx6ul() {
	install -m 0755 ${WORKDIR}/caam_jr0_wakeup.sh ${D}${sysconfdir}/init.d/caam_jr0_wakeup.sh
	update-rc.d -r ${D} caam_jr0_wakeup.sh start 20 S .
}

PACKAGE_ARCH = "${MACHINE_ARCH}"
