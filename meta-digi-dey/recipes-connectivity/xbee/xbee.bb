# Copyright (C) 2019,2020 Digi International Inc.

SUMMARY = "Digi XBee initialization"
DESCRIPTION = "Initialization scripts for XBee hardware of Digi boards"
SECTION = "base"
LICENSE = "GPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

SRC_URI = " \
    file://xbee-init \
    file://xbee-init.service \
"
S = "${WORKDIR}"

inherit systemd update-rc.d

do_install() {
	install -d ${D}${sysconfdir}/init.d/
	install -m 0755 ${WORKDIR}/xbee-init ${D}${sysconfdir}/
	ln -sf /etc/xbee-init ${D}${sysconfdir}/init.d/xbee-init
	sed -i -e "s/##XBEE_RESET_N_GPIO##/${XBEE_RESET_N_GPIO}/g" \
	       -e "s/##XBEE_SLEEP_RQ_GPIO##/${XBEE_SLEEP_RQ_GPIO}/g" \
	       ${D}${sysconfdir}/xbee-init

	install -d ${D}${systemd_unitdir}/system/
	install -m 0644 ${WORKDIR}/xbee-init.service ${D}${systemd_unitdir}/system/
}

PACKAGES =+ "${PN}-init"
FILES_${PN}-init = " \
    ${sysconfdir}/xbee-init \
    ${sysconfdir}/init.d/xbee-init \
    ${systemd_unitdir}/system/xbee-init.service \
"
INITSCRIPT_PACKAGES += "${PN}-init"
INITSCRIPT_NAME_${PN}-init = "xbee-init"
INITSCRIPT_PARAMS_${PN}-init = "start 19 2 3 4 5 . stop 21 0 1 6 ."

SYSTEMD_PACKAGES = "${PN}-init"
SYSTEMD_SERVICE_${PN}-init = "xbee-init.service"

PACKAGE_ARCH = "${MACHINE_ARCH}"
COMPATIBLE_MACHINE = "(ccimx8x|ccimx8m)"
