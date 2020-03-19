# Copyright (C) 2013-2020 Digi International.

FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

PACKAGECONFIG ?= "openssl"

SRC_URI += " \
    file://0001-wpa_supplicant-enable-control-socket-interface-when-.patch \
    ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'file://0002-wpa_supplicant-systemd-enable-control-socket-interfa.patch', '', d)} \
    file://0003-mesh-encapsulate-VHT-property-with-the-proper-CONFIG.patch \
    file://wpa_supplicant_p2p.conf \
"

do_install_append() {
	install -m 600 ${WORKDIR}/wpa_supplicant_p2p.conf ${D}${sysconfdir}/wpa_supplicant_p2p.conf
	sed -i -e "s,##WLAN_P2P_DEVICE_NAME##,${WLAN_P2P_DEVICE_NAME},g" \
	       ${D}${sysconfdir}/wpa_supplicant_p2p.conf
}

do_install_append_ccimx6sbc() {
    # Customize supplicant file
    cat <<EOF >>${D}${sysconfdir}/wpa_supplicant.conf

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
}

PACKAGE_ARCH = "${MACHINE_ARCH}"
