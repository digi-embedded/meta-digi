# Copyright (C) 2013-2020 Digi International.

FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

PACKAGECONFIG ?= "openssl"

SRC_URI += " \
    file://0001-wpa_supplicant-enable-control-socket-interface-when-.patch \
    ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'file://0002-wpa_supplicant-systemd-enable-control-socket-interfa.patch', '', d)} \
    file://wpa_supplicant_p2p.conf \
"

SRC_URI_append_ccimx6sbc = " file://wpa_supplicant_p2p.conf_atheros"

do_install_append() {
	install -m 600 ${WORKDIR}/wpa_supplicant_p2p.conf ${D}${sysconfdir}/wpa_supplicant_p2p.conf
	sed -i -e "s,##WLAN_P2P_DEVICE_NAME##,${WLAN_P2P_DEVICE_NAME},g" \
	       ${D}${sysconfdir}/wpa_supplicant_p2p.conf
}

do_install_append_ccimx6sbc() {
	# Install atheros variant of the p2p .conf file
	install -m 600 ${WORKDIR}/wpa_supplicant_p2p.conf_atheros ${D}${sysconfdir}/wpa_supplicant_p2p.conf_atheros
	sed -i -e "s,##WLAN_P2P_DEVICE_NAME##,${WLAN_P2P_DEVICE_NAME},g" \
	       ${D}${sysconfdir}/wpa_supplicant_p2p.conf_atheros
}

pkg_postinst_ontarget_${PN}_ccimx6sbc() {
	# Only execute the script on wireless ccimx6 platforms
	if [ -e "/proc/device-tree/wireless/mac-address" ]; then
		for id in $(find /sys/devices -name modalias -print0 | xargs -0 sort -u -z | grep sdio); do
			if [[ "$id" == "sdio:c00v0271d0301" ]] ; then
				# Customize supplicant file
				cat <<EOF >>/etc/wpa_supplicant.conf

# -- SoftAP mode
# ap_scan=2
# network={
# 	ssid="ath6kl-ap"
# 	mode=2
# 	frequency=2412
# 	key_mgmt=WPA-PSK
# 	proto=RSN
# 	pairwise=CCMP
# 	psk="12345678"
# }

EOF
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
