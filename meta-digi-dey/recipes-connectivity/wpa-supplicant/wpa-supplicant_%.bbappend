# Copyright (C) 2013 Digi International.

FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

PACKAGECONFIG ?= "openssl"

SRC_URI_append_ccimx6ul = "file://wpa_supplicant_p2p.conf"

do_install_append_ccimx6ul() {
	install -m 600 ${WORKDIR}/wpa_supplicant_p2p.conf ${D}${sysconfdir}/wpa_supplicant_p2p.conf
}

PACKAGE_ARCH = "${MACHINE_ARCH}"
