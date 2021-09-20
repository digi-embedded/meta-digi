# Copyright (C) 2013-2021 Digi International.

FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

PACKAGECONFIG ?= "openssl"

SRC_URI += " \
    file://0001-wpa_supplicant-enable-control-socket-interface-when-.patch \
    file://wpa_supplicant_p2p.conf \
"

SRC_URI_append_ccimx6sbc = " file://wpa_supplicant_p2p.conf_atheros"

do_install_append() {
	install -m 600 ${WORKDIR}/wpa_supplicant_p2p.conf ${D}${sysconfdir}/wpa_supplicant_p2p.conf
}

do_install_append_ccimx6sbc() {
	# Install atheros variant of the p2p .conf file
	install -m 600 ${WORKDIR}/wpa_supplicant_p2p.conf_atheros ${D}${sysconfdir}/wpa_supplicant_p2p.conf_atheros
}

pkg_postinst_${PN}_ccimx6sbc() {
	if [ -n "$D" ]; then
		exit 1
	fi

	# Since we're overwriting the post-installation script in poky, copy it
	# here to avoid losing it
	killall -q -HUP dbus-daemon || true

	# Only execute the script on wireless ccimx6 platforms
	if [ -e "/proc/device-tree/wireless/mac-address" ]; then
		for id in $(find /sys/devices -name modalias -print0 | xargs -0 sort -u -z | grep sdio); do
			if [[ "$id" == "sdio:c00v0271d0301" ]] ; then
				mv /etc/wpa_supplicant_p2p.conf_atheros /etc/wpa_supplicant_p2p.conf
				break
			elif [[ "$id" == "sdio:c00v0271d050A" ]] ; then
				rm /etc/wpa_supplicant_p2p.conf_atheros
				break
			fi
		done
	fi
}

PACKAGE_ARCH = "${MACHINE_ARCH}"
