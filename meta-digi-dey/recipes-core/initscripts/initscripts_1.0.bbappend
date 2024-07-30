# Copyright (C) 2013-2017, Digi International Inc.

FILESEXTRAPATHS:prepend := "${THISDIR}/${BP}:"

SRC_URI += "file://devtmpfs.sh"

SRC_URI:append:ccimx6ul = " file://caam_jr0_wakeup.sh"

do_install:append() {
	install -m 755 ${WORKDIR}/devtmpfs.sh ${D}${sysconfdir}/init.d/devtmpfs.sh
	update-rc.d -r ${D} devtmpfs.sh start 03 S .
}

do_install:append:ccimx6ul() {
	install -m 0755 ${WORKDIR}/caam_jr0_wakeup.sh ${D}${sysconfdir}/init.d/caam_jr0_wakeup.sh
	update-rc.d -r ${D} caam_jr0_wakeup.sh start 20 S .
}

PACKAGE_ARCH = "${MACHINE_ARCH}"
