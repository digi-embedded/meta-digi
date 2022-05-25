# Copyright (C) 2013-2022 Digi International.

FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

PACKAGECONFIG ?= "openssl"

SRC_URI += " \
    file://0001-wpa_supplicant-enable-control-socket-interface-when-.patch \
    ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'file://0002-wpa_supplicant-systemd-enable-control-socket-interfa.patch', '', d)} \
    file://wpa_supplicant_p2p.conf \
"

# We maintain all patches from Infineon release, but do not apply the patches that
# touches files under 'hostapd' directory, as that directory is not available in the
# wpa_supplicant package from a release tarball.
MURATA_COMMON_PATCHES = " \
    file://murata/0001-wpa_supplicant-Support-4-way-handshake-offload-for-F.patch \
    file://murata/0002-wpa_supplicant-Notify-Neighbor-Report-for-driver-tri.patch \
    file://murata/0003-nl80211-Report-connection-authorized-in-EVENT_ASSOC.patch \
    file://murata/0004-wpa_supplicant-Add-PMKSA-cache-for-802.1X-4-way-hand.patch \
    file://murata/0005-OpenSSL-Fix-build-with-OpenSSL-1.0.1.patch \
    file://murata/0006-nl80211-Check-SAE-authentication-offload-support.patch \
    file://murata/0007-SAE-Pass-SAE-password-on-connect-for-SAE-authenticat.patch \
    file://murata/0008-nl80211-Support-4-way-handshake-offload-for-WPA-WPA2.patch \
    file://murata/0009-AP-Support-4-way-handshake-offload-for-WPA-WPA2-PSK.patch \
    file://murata/0010-nl80211-Support-SAE-authentication-offload-in-AP-mod.patch \
    file://murata/0011-SAE-Support-SAE-authentication-offload-in-AP-mode.patch \
    file://murata/0012-DPP-Do-more-condition-test-for-AKM-type-DPP-offload.patch \
    file://murata/0013-non-upstream-defconfig_base-Add-Infineon-default-con.patch \
    file://murata/0014-non-upstream-defconfig_base-Add-Infineon-default-con.patch;apply=no \
    file://murata/0015-Add-CONFIG_WPA3_SAE_AUTH_EARLY_SET-flags-and-codes-f.patch \
    file://murata/0016-Add-CONFIG_WPA3_SAE_AUTH_EARLY_SET-flags-and-codes-s.patch;apply=no \
    file://murata/0017-SAE-Set-the-right-WPA-Versions-for-FT-SAE-key-manage.patch \
    file://murata/0018-wpa_supplicant-Support-WPA_KEY_MGMT_FT-for-eapol-off.patch \
    file://murata/0019-wpa_supplicant-suppress-deauth-for-PMKSA-caching-dis.patch \
    file://murata/0020-Fix-to-check-Invalid-GTK-IE-length-in-M3-at-STA.patch \
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
