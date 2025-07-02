# Copyright (C) 2023-2025 Digi International Inc.

FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

SRC_URI:append = " \
    file://81-iw612-wifi.rules \
    file://load_iw612.sh \
    file://watch_regdomain.sh \
    file://watch-regdomain.service \
    file://watch-regdomain.timer \
    file://0001-mxm_wifiex-do-not-process-countryIE-internally.patch \
"

do_install:append () {
	install -d ${D}${sysconfdir}/udev/rules.d
	install -m 0644 ${WORKDIR}/81-iw612-wifi.rules ${D}${sysconfdir}/udev/rules.d/
	install -d ${D}${sysconfdir}/udev/scripts
	install -m 0777 ${WORKDIR}/load_iw612.sh ${D}${sysconfdir}/udev/scripts/

	if ${@bb.utils.contains('DISTRO_FEATURES','systemd','true','false',d)}; then
		install -d ${D}${systemd_system_unitdir}
		install -m 0644 ${WORKDIR}/watch-regdomain.service ${D}${systemd_system_unitdir}/watch-regdomain.service
		install -m 0644 ${WORKDIR}/watch-regdomain.timer ${D}${systemd_system_unitdir}/watch-regdomain.timer
	fi

	install -d ${D}${sbindir}
	install -m 0755 ${WORKDIR}/watch_regdomain.sh ${D}${sbindir}/
}

FILES:${PN}:append = " \
	${sysconfdir}/udev/rules.d \
	${sysconfdir}/udev/scripts \
	${sbindir}/watch_regdomain.sh \
	${systemd_system_unitdir}/watch-regdomain.service \
	${systemd_system_unitdir}/watch-regdomain.timer \
"

RDEPENDS:${PN}:append = " firmware-murata-nxp"
