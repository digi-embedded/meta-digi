# Copyright (C) 2015-2022, Digi International Inc.

SRC_URI += " \
    file://bluetooth.service-add-customizations.patch \
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

SRC_URI:append:ccimx6ul = " ${QCA65XX_COMMON_PATCHES}"
SRC_URI:append:ccimx6 = " ${QCA65XX_COMMON_PATCHES}"

SRC_URI:append:ccimx6sbc = " \
    file://main.conf_atheros \
"

PACKAGECONFIG:append = " experimental"

do_install:append() {
	install -m 0644 ${WORKDIR}/main.conf ${D}${sysconfdir}/bluetooth/
	sed -i -e "s,##BT_DEVICE_NAME##,${BT_DEVICE_NAME},g" \
		${D}${sysconfdir}/bluetooth/main.conf
	if [ -n "${@bb.utils.contains('PACKAGECONFIG', 'experimental', 'experimental', '', d)}" ]; then
		sed -i '/^SSD_OPTIONS/a SSD_OPTIONS="${SSD_OPTIONS} --experimental"' ${D}${INIT_D_DIR}/bluetooth
	fi
}

do_install:append:ccimx6sbc() {
	install -m 0644 ${WORKDIR}/main.conf_atheros ${D}${sysconfdir}/bluetooth/
	sed -i -e "s,##BT_DEVICE_NAME##,${BT_DEVICE_NAME},g" \
		${D}${sysconfdir}/bluetooth/main.conf_atheros
}

pkg_postinst_ontarget:${PN}:ccimx6sbc() {
	# Only execute the script on wireless ccimx6 platforms
	if [ -e "/proc/device-tree/bluetooth/mac-address" ]; then
		for id in $(find /sys/devices -name modalias -print0 | xargs -0 sort -u -z | grep sdio); do
			if [[ "$id" == "sdio:c00v0271d0301" ]] ; then
				mv /etc/bluetooth/main.conf_atheros /etc/bluetooth/main.conf
				break
			elif [[ "$id" == "sdio:c00v0271d050A" ]] ; then
				rm /etc/bluetooth/main.conf_atheros
				break
			fi
		done
	fi
}

FILES:${PN} += " ${sysconfdir}/bluetooth/main.conf*"

PACKAGE_ARCH = "${MACHINE_ARCH}"
