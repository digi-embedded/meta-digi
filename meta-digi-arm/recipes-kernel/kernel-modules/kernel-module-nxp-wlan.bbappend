# Copyright (C) 2023,2024, Digi International Inc.

FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

SRC_URI:append = " \
    file://81-iw612-wifi.rules \
    file://load_iw612.sh \
    file://0001-issue-fix-wlan_src_driver_patch_release_base_version.patch \
"

SRCBRANCH = "lf-6.6.36_2.1.0"
SRCREV = "e5c9a169d7b7a441a20d2cf10a9752e249b71cff"
S = "${WORKDIR}/git"

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
