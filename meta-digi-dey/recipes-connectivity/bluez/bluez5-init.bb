# Copyright (C) 2022-2023, Digi International Inc.

SUMMARY = "Bluetooth init scripts"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"

SRC_URI = " \
    file://bluetooth-init \
    file://bluetooth-init.service \
"

SRC_URI:append:ccimx6sbc = " \
    file://bluetooth-init_atheros \
"

inherit update-rc.d systemd

do_install() {
	# INITSCRIPT
	install -d ${D}${sysconfdir}/init.d/
	install -m 0755 ${WORKDIR}/bluetooth-init ${D}${sysconfdir}/bluetooth-init
	ln -sf /etc/bluetooth-init ${D}${sysconfdir}/init.d/bluetooth-init
	# SYSTEMD
	install -d ${D}${systemd_unitdir}/system/
	install -m 0644 ${WORKDIR}/bluetooth-init.service ${D}${systemd_unitdir}/system/bluetooth-init.service
}

do_install:append:ccimx6sbc() {
	install -m 0755 ${WORKDIR}/bluetooth-init_atheros ${D}${sysconfdir}/bluetooth-init_atheros
}

pkg_postinst_ontarget:${PN}:ccimx6sbc() {
	# Only execute the script on wireless ccimx6 platforms
	if [ -e "/proc/device-tree/bluetooth/mac-address" ]; then
		for id in $(find /sys/devices -name modalias -print0 | xargs -0 sort -u -z | grep sdio); do
			if [[ "$id" == "sdio:c00v0271d0301" ]] ; then
				mv /etc/bluetooth-init_atheros /etc/bluetooth-init
				break
			elif [[ "$id" == "sdio:c00v0271d050A" ]] ; then
				rm /etc/bluetooth-init_atheros
				break
			fi
		done
	fi
}

FILES:${PN} = " ${sysconfdir}/bluetooth-init* \
                ${sysconfdir}/init.d/bluetooth-init \
                ${systemd_unitdir}/system/bluetooth-init.service \
"

INITSCRIPT_PACKAGES += "${PN}"
INITSCRIPT_NAME:${PN} = "bluetooth-init"
INITSCRIPT_PARAMS:${PN} = "start 19 2 3 4 5 . stop 21 0 1 6 ."

SYSTEMD_SERVICE:${PN} = "bluetooth-init.service"

# 'bluetooth-init' script uses '/etc/init.d/functions'
RDEPENDS:${PN} = "initscripts-functions"

# IW61x Bluetooth support requires the WiFi FW support
RDEPENDS:${PN}:append:ccimx93 = " firmware-nxp-wifi-nxpiw612"

PACKAGE_ARCH = "${MACHINE_ARCH}"
COMPATIBLE_MACHINE = "(ccimx6$|ccimx6ul|ccimx8x|ccimx8mn|ccimx8mm|ccimx93)"
