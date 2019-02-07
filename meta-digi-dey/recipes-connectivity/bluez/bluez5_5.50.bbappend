# Copyright (C) 2015-2018 Digi International.

FILESEXTRAPATHS_prepend := "${THISDIR}/${BP}:"

SRC_URI += " \
    file://bluetooth-init \
    file://bluetooth-init.service \
    file://main.conf \
    file://0001-hcitool-do-not-show-unsupported-refresh-option.patch \
    file://0002-hcitool-increase-the-shown-connection-limit-to-20.patch \
    file://0003-port-test-discovery-to-python3.patch \
"

QCA65XX_COMMON_PATCHES = " \
    file://0004-QCA_bluetooth_chip_support.patch \
    file://0005-hciattach_rome-Respect-the-user-indication-for-noflo.patch \
    file://0006-hciattach-If-the-user-supplies-a-bdaddr-use-it.patch \
    file://0007-hciattach-Add-verbosity-option.patch \
"

SRC_URI_append_ccimx6ul = " ${QCA65XX_COMMON_PATCHES}"
SRC_URI_append_ccimx6qpsbc = " ${QCA65XX_COMMON_PATCHES}"
SRC_URI_append_ccimx8x = " ${QCA65XX_COMMON_PATCHES}"

inherit update-rc.d

do_install_append() {
	install -d ${D}${sysconfdir}/init.d/
	install -m 0755 ${WORKDIR}/bluetooth-init ${D}${sysconfdir}/bluetooth-init
	ln -sf /etc/bluetooth-init ${D}${sysconfdir}/init.d/bluetooth-init
	install -d ${D}${systemd_unitdir}/system/
	install -m 0644 ${WORKDIR}/bluetooth-init.service ${D}${systemd_unitdir}/system/bluetooth-init.service
	install -m 0644 ${WORKDIR}/main.conf ${D}${sysconfdir}/bluetooth/
}

PACKAGES =+ "${PN}-init"
PACKAGECONFIG_append = " health-profiles \
    mesh \
    btpclient \
"

FILES_${PN} += " ${sysconfdir}/bluetooth/main.conf"
FILES_${PN}-init = " ${sysconfdir}/bluetooth-init \
                     ${sysconfdir}/init.d/bluetooth-init \
                     ${systemd_unitdir}/system/bluetooth-init.service \
"

INITSCRIPT_PACKAGES += "${PN}-init"
INITSCRIPT_NAME_${PN}-init = "bluetooth-init"
INITSCRIPT_PARAMS_${PN}-init = "start 19 2 3 4 5 . stop 21 0 1 6 ."

SYSTEMD_SERVICE_${PN}-init = "bluetooth-init.service"

PACKAGE_ARCH = "${MACHINE_ARCH}"
