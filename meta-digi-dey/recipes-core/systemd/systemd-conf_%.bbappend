#  Copyright (C) 2019-2023 by Digi International Inc.

FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

SRC_URI:append = " \
    file://system.conf-digi \
"

SRC_URI:append:ccmp1 = " \
    file://logind.conf-digi \
"

do_install:append() {
	install -D -m0644 ${WORKDIR}/system.conf-digi ${D}${systemd_unitdir}/system.conf.d/01-${PN}.conf
}

do_install:append:ccmp1() {
	install -D -m0644 ${WORKDIR}/logind.conf-digi ${D}${systemd_unitdir}/logind.conf.d/01-${PN}.conf
}
