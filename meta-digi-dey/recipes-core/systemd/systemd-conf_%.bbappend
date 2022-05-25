#  Copyright (C) 2019,2020 by Digi International Inc.

FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

SRC_URI:append = " \
    file://system.conf-imx \
"

do_install:append() {
	install -D -m0644 ${WORKDIR}/system.conf-imx ${D}${systemd_unitdir}/system.conf.d/01-${PN}.conf
}
