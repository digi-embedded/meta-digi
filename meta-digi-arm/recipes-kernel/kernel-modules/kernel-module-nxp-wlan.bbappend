# Copyright (C) 2023 Digi International.

FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

SRC_URI:append = " \
    file://81-iw612-wifi.rules \
    file://load_iw612.sh \
"

do_install:append () {
	install -d ${D}${sysconfdir}/udev/rules.d
	install -m 0644 ${WORKDIR}/81-iw612-wifi.rules ${D}${sysconfdir}/udev/rules.d/
	install -d ${D}${sysconfdir}/udev/scripts
	install -m 0777 ${WORKDIR}/load_iw612.sh ${D}${sysconfdir}/udev/scripts/
}

FILES:${PN}:append = " \
	${sysconfdir}/udev/rules.d \
	${sysconfdir}/udev/scripts \
"

RDEPENDS:${PN}:remove = "wireless-tools"
RDEPENDS:${PN}:append = " firmware-murata-nxp"
