# Copyright (C) 2013-2022 Digi International.

FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

PACKAGECONFIG ?= "openssl"

SRC_URI += " \
    file://0001-wpa_supplicant-enable-control-socket-interface-when-.patch \
    ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'file://0002-wpa_supplicant-systemd-enable-control-socket-interfa.patch', '', d)} \
    file://0003-mesh-encapsulate-VHT-property-with-the-proper-CONFIG.patch \
    file://wpa_supplicant_p2p.conf \
"

MURATA_COMMON_PATCHES = " \
    file://murata/0001-wpa_supplicant-Support-4-way-handshake-offload-for-F.patch;apply=yes \
    file://murata/0002-wpa_supplicant-Notify-Neighbor-Report-for-driver-tri.patch;apply=yes \
    file://murata/0003-nl80211-Report-connection-authorized-in-EVENT_ASSOC.patch;apply=yes \
    file://murata/0004-wpa_supplicant-Add-PMKSA-cache-for-802.1X-4-way-hand.patch;apply=yes \
    file://murata/0005-Sync-with-mac80211-next.git-include-uapi-linux-nl802.patch;apply=yes \
    file://murata/0006-nl80211-Check-SAE-authentication-offload-support.patch;apply=yes \
    file://murata/0007-SAE-Pass-SAE-password-on-connect-for-SAE-authenticat.patch;apply=yes \
    file://murata/0008-OpenSSL-Fix-build-with-OpenSSL-1.0.1.patch;apply=yes \
    file://murata/0009-non-upstream-Sync-nl80211.h-for-PSK-4-way-HS-offload.patch;apply=yes \
    file://murata/0010-nl80211-Support-4-way-handshake-offload-for-WPA-WPA2.patch;apply=yes \
    file://murata/0011-AP-Support-4-way-handshake-offload-for-WPA-WPA2-PSK.patch;apply=yes \
    file://murata/0012-nl80211-Support-SAE-authentication-offload-in-AP-mod.patch;apply=yes \
    file://murata/0013-SAE-Support-SAE-authentication-offload-in-AP-mode.patch;apply=yes \
    file://murata/0014-P2P-Fix-P2P-authentication-failure-due-to-AP-mode-4-.patch;apply=yes \
    file://murata/0016-DPP-Do-more-condition-test-for-AKM-type-DPP-offload.patch;apply=yes \
    file://murata/0017-hostapd-Fix-PMF-connection-issue.patch;apply=yes \
    file://murata/0018-AP-Set-Authenticator-state-properly-for-PSK-4-way-ha.patch;apply=yes \
    file://murata/0019-wpa-supplicant-defconfig-Set-to-Cypress-default-configuration.patch;apply=yes \
"

SRC_URI:append:ccimx6sbc = " file://wpa_supplicant_p2p.conf_atheros"
SRC_URI:append:ccmp1 = " ${MURATA_COMMON_PATCHES}"
SRC_URI:append:ccimx8mp = " ${MURATA_COMMON_PATCHES}"

do_install:append() {
	install -m 600 ${WORKDIR}/wpa_supplicant_p2p.conf ${D}${sysconfdir}/wpa_supplicant_p2p.conf
	sed -i -e "s,##WLAN_P2P_DEVICE_NAME##,${WLAN_P2P_DEVICE_NAME},g" \
	       ${D}${sysconfdir}/wpa_supplicant_p2p.conf
}

do_install:append:ccimx6sbc() {
	# Install atheros variant of the p2p .conf file
	install -m 600 ${WORKDIR}/wpa_supplicant_p2p.conf_atheros ${D}${sysconfdir}/wpa_supplicant_p2p.conf_atheros
	sed -i -e "s,##WLAN_P2P_DEVICE_NAME##,${WLAN_P2P_DEVICE_NAME},g" \
	       ${D}${sysconfdir}/wpa_supplicant_p2p.conf_atheros
}

pkg_postinst_ontarget:${PN}:ccimx6sbc() {
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
