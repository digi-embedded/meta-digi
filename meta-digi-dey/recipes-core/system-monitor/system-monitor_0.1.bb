# Copyright (C) 2019, Digi International Inc.
SUMMARY = "Install and start a systemd service"
LICENSE = "MPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MPL-2.0;md5=815ca599c9df247a0c7f619bab123dad"

SRC_URI = " \
    file://connectivity-check \
    file://recover-bridge-action \
    file://system-monitor.sh \
    file://system-monitor.service \
"
S = "${WORKDIR}"

inherit systemd features_check

REQUIRED_DISTRO_FEATURES= "systemd"

SYSTEMD_SERVICE:${PN} = "system-monitor.service"

# The system-monitor.sh script is an example that needs to be customized.
# This service also needs to be manually enabled.
SYSTEMD_AUTO_ENABLE ?= "disable"

do_install() {
	install -d ${D}${bindir}
	install -m 0755 ${WORKDIR}/system-monitor.sh ${D}${bindir}

	install -d ${D}${systemd_unitdir}/system/
	install -m 0644 ${WORKDIR}/system-monitor.service ${D}${systemd_unitdir}/system/

	install -d ${D}${sysconfdir}/system-monitor/check.d ${D}${sysconfdir}/system-monitor/recover-action.d
	install -m 0755 ${WORKDIR}/connectivity-check ${D}${sysconfdir}/system-monitor/check.d
	install -m 0755 ${WORKDIR}/recover-bridge-action ${D}${sysconfdir}/system-monitor/recover-action.d
}

FILES:${PN} += "${systemd_unitdir}/system/system-monitor.service"
