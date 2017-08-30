# Copyright (C) 2013-2017 Digi International.

FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

PACKAGECONFIG ?= "openssl"

SRC_URI += " \
    file://0001-wpa_supplicant-enable-control-socket-interface-when-.patch \
    file://wpa_supplicant_p2p.conf \
"

do_install_append() {
	install -m 600 ${WORKDIR}/wpa_supplicant_p2p.conf ${D}${sysconfdir}/wpa_supplicant_p2p.conf
}

PACKAGE_ARCH = "${MACHINE_ARCH}"
