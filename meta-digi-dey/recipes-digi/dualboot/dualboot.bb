# Copyright (C) 2021-2023, Digi International Inc.

SUMMARY = "Digi Embedded Yocto Dual boot support"
SECTION = "base"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"

SOC_SIGN_DEPENDS = " \
    ${@oe.utils.conditional('DEY_SOC_VENDOR', 'NXP', 'trustfence-cst-native', '', d)} \
"
DEPENDS += "${@oe.utils.conditional('TRUSTFENCE_SIGN', '1', \
		'openssl-native ' \
		'trustfence-sign-tools-native ' \
		'${SOC_SIGN_DEPENDS}', '', d)}"

SRC_URI = " \
    file://dualboot-init \
    file://update-firmware \
    file://firmware-update-check.service \
"

S = "${WORKDIR}"

inherit systemd update-rc.d

do_configure[noexec] = "1"
do_compile[noexec] = "1"

do_install() {
	install -d ${D}${sysconfdir}/init.d/
	install -m 0755 ${WORKDIR}/dualboot-init ${D}${sysconfdir}/dualboot-init
	ln -sf /etc/dualboot-init ${D}${sysconfdir}/init.d/dualboot-init

	install -d ${D}${bindir}
	install -m 0755 ${WORKDIR}/update-firmware ${D}${bindir}

	install -d ${D}${systemd_unitdir}/system/
	install -m 0644 ${WORKDIR}/firmware-update-check.service ${D}${systemd_unitdir}/system/
}

FILES:${PN} += " \
    ${sysconfdir}/dualboot-init \
    ${sysconfdir}/init.d/dualboot-init \
    ${bindir}/update-firmware \
    ${systemd_unitdir}/system/firmware-update-check.service \
    ${@oe.utils.conditional('TRUSTFENCE_SIGN', '1', '${sysconfdir}/ssl/certs/key.pub', '', d)} \
"

INITSCRIPT_NAME = "dualboot-init"
INITSCRIPT_PARAMS = "start 19 2 3 4 5 . stop 21 0 1 6 ."

SYSTEMD_SERVICE:${PN} = "firmware-update-check.service"

RDEPENDS:${PN} += "swupdate"
