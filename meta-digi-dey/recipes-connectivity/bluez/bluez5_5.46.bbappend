# Copyright (C) 2015-2021 Digi International.

FILESEXTRAPATHS_prepend := "${THISDIR}/${BP}:"

SRC_URI += " \
    file://bluetooth-init \
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
SRC_URI_append_ccimx6 = " ${QCA65XX_COMMON_PATCHES}"
SRC_URI_append_ccimx8x = " ${QCA65XX_COMMON_PATCHES}"

SRC_URI_append_ccimx6sbc = " \
    file://bluetooth-init_atheros \
    file://main.conf_atheros \
"

inherit update-rc.d

do_install_append() {
	install -d ${D}${sysconfdir}/init.d/
	install -m 0755 ${WORKDIR}/bluetooth-init ${D}${sysconfdir}/init.d/bluetooth-init
	install -m 0644 ${WORKDIR}/main.conf ${D}${sysconfdir}/bluetooth/
}

do_install_append_ccimx6sbc() {
	install -m 0755 ${WORKDIR}/bluetooth-init_atheros ${D}${sysconfdir}/init.d/bluetooth-init_atheros
	install -m 0644 ${WORKDIR}/main.conf_atheros ${D}${sysconfdir}/bluetooth/
}

pkg_postinst_${PN}_ccimx6sbc() {
	if [ -n "$D" ]; then
		exit 1
	fi

	# Only execute the script on wireless ccimx6 platforms
	if [ -e "/proc/device-tree/bluetooth/mac-address" ]; then
		for id in $(find /sys/devices -name modalias -print0 | xargs -0 sort -u -z | grep sdio); do
			if [[ "$id" == "sdio:c00v0271d0301" ]] ; then
				mv /etc/init.d/bluetooth-init_atheros /etc/init.d/bluetooth-init
				mv /etc/bluetooth/main.conf_atheros /etc/bluetooth/main.conf
				break
			elif [[ "$id" == "sdio:c00v0271d050A" ]] ; then
				rm /etc/init.d/bluetooth-init_atheros
				rm /etc/bluetooth/main.conf_atheros
				break
			fi
		done
	fi
}

PACKAGES =+ "${PN}-init"
PACKAGECONFIG_append = " health-profiles"

FILES_${PN} += " ${sysconfdir}/bluetooth/main.conf*"
FILES_${PN}-init = "${sysconfdir}/init.d/bluetooth-init*"

INITSCRIPT_PACKAGES += "${PN}-init"
INITSCRIPT_NAME_${PN}-init = "bluetooth-init"
INITSCRIPT_PARAMS_${PN}-init = "start 19 2 3 4 5 . stop 21 0 1 6 ."

PACKAGE_ARCH = "${MACHINE_ARCH}"
