# Copyright (C) 2015-2018 Digi International.

FILESEXTRAPATHS_prepend := "${THISDIR}/${BP}:"

SRC_URI += " \
    file://bluetooth-init \
    file://bluetooth-init.service \
    file://main.conf \
    file://0001-hcitool-do-not-show-unsupported-refresh-option.patch \
    file://0002-hcitool-increase-the-shown-connection-limit-to-20.patch \
    file://0003-port-test-discovery-to-python3.patch \
    file://0004-example-gatt-server-update-example-to-master-version.patch \
    file://0005-core-Prefer-BR-EDR-over-LE-if-it-set-in-advertisemen.patch \
    file://0006-core-device-Fix-not-connecting-services-properly.patch \
    file://0007-core-device-Fix-marking-auto-connect-flag.patch \
    file://0008-core-device-Prefer-bonded-bearers-when-connecting.patch \
    file://0009-input-hog-Use-.accept-and-.disconnect-instead-of-att.patch \
    file://0010-src-device-Free-bonding-while-failed-to-pair-device.patch \
    file://0011-core-Fix-BR-EDR-pairing-for-dual-mode-devices.patch \
"

QCA65XX_COMMON_PATCHES = " \
    file://0012-QCA_bluetooth_chip_support.patch \
    file://0013-hciattach_rome-Respect-the-user-indication-for-noflo.patch \
    file://0014-hciattach-If-the-user-supplies-a-bdaddr-use-it.patch \
    file://0015-hciattach-Add-verbosity-option.patch \
    file://0016-bluetooth-Disable-bluetooth-low-power-mode-functionality.patch \
"

SRC_URI_append_ccimx6ul = " ${QCA65XX_COMMON_PATCHES}"
SRC_URI_append_ccimx6qpsbc = " ${QCA65XX_COMMON_PATCHES}"

inherit update-rc.d

PACKAGECONFIG_append = " experimental"

do_install_append() {
	install -d ${D}${sysconfdir}/init.d/
	install -m 0755 ${WORKDIR}/bluetooth-init ${D}${sysconfdir}/bluetooth-init
	ln -sf /etc/bluetooth-init ${D}${sysconfdir}/init.d/bluetooth-init
	install -d ${D}${systemd_unitdir}/system/
	install -m 0644 ${WORKDIR}/bluetooth-init.service ${D}${systemd_unitdir}/system/bluetooth-init.service
	install -m 0644 ${WORKDIR}/main.conf ${D}${sysconfdir}/bluetooth/
	if [ -n "${@bb.utils.contains('PACKAGECONFIG', 'experimental', 'experimental', '', d)}" ]; then
		sed -i '/^SSD_OPTIONS/a SSD_OPTIONS="${SSD_OPTIONS} --experimental"' ${D}${INIT_D_DIR}/bluetooth
	fi
}

PACKAGES =+ "${PN}-init"

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
