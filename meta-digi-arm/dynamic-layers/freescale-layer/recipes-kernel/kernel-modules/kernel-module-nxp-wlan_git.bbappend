# Copyright (C) 2023-2025 Digi International Inc.

FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

SRC_URI:append = " \
    file://81-iw612-wifi.rules \
    file://load_iw612.sh \
    file://0001-issue-fix-wlan_src_driver_patch_release_lf-6.6.52-2..patch \
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

RDEPENDS:${PN}:append = " firmware-murata-nxp"
