# Copyright (C) 2013-2024, Digi International Inc.

FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

PACKAGECONFIG ?= "openssl"

SRC_URI += " \
    file://0001-wpa_supplicant-enable-control-socket-interface-when-.patch \
    ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'file://0002-wpa_supplicant-systemd-enable-control-socket-interfa.patch', '', d)} \
    file://wpa_supplicant_p2p.conf \
"

# Patch series from Murata release
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
    file://murata/0014-CVE_2019_9501-Fix-to-check-Invalid-GTK-IE-length-in-.patch \
    file://murata/0015-Add-CONFIG_WPA3_SAE_AUTH_EARLY_SET-flags-and-codes.murata.patch \
    file://murata/0016-SAE-Set-the-right-WPA-Versions-for-FT-SAE-key-manage.patch \
    file://murata/0017-wpa_supplicant-Support-WPA_KEY_MGMT_FT-for-eapol-off.patch \
    file://murata/0018-wpa_supplicant-suppress-deauth-for-PMKSA-caching-dis.patch \
    file://murata/0019-Fix-for-PMK-expiration-issue-through-supplicant.murata.patch \
    file://murata/0020-SAE-Drop-PMKSA-cache-after-receiving-specific-deauth.patch \
    file://murata/0021-Avoid-deauthenticating-STA-if-the-reason-for-freeing.patch \
    file://murata/0022-wpa_supplicant-support-bgscan.patch \
    file://murata/0023-non-upstream-wl-cmd-create-interface-to-support-driv.murata.patch \
    file://murata/0024-non-upstream-wl-cmd-create-wl_do_cmd-as-an-entry-doi.patch \
    file://murata/0025-non-upstream-wl-cmd-create-ops-table-to-do-wl-comman.patch \
    file://murata/0026-non-upstream-wl-cmd-add-more-compile-flag.patch \
    file://murata/0027-Fix-dpp-config-parameter-setting.patch \
    file://murata/0028-DPP-Resolving-failure-of-dpp-configurator-exchange-f.patch \
    file://murata/0029-Enabling-SUITEB192-and-SUITEB-compile-options.patch \
    file://murata/0030-DPP-Enabling-CLI_EDIT-option-for-enrollee-plus-respo.patch \
    file://murata/0031-P2P-Fixes-Scan-trigger-failed-once-GC-invited-by-GO.patch \
    file://murata/0032-non-upstream-SAE-disconnect-after-PMKSA-cache-expire.patch \
    file://murata/0033-Add-support-for-beacon-loss-roaming.patch \
    file://murata/0034-wpa_supplicant-Set-PMKSA-to-driver-while-key-mgmt-is.patch \
    file://murata/0035-nl80211-Set-NL80211_SCAN_FLAG_COLOCATED_6GHZ-in-scan.patch \
    file://murata/0036-scan-Add-option-to-disable-6-GHz-collocated-scanning.patch \
    file://murata/0037-Enabling-OWE-in-wpa_supplicant.patch \
    file://murata/0038-Add-link-loss-timer-on-beacon-loss.patch \
    file://murata/0039-FT-Sync-nl80211-ext-feature-index.patch \
    file://murata/0040-nl80211-Introduce-a-vendor-header-for-vendor-NL-ifac.patch \
    file://murata/0041-add-support-to-offload-TWT-setup-request-handling-to.patch \
    file://murata/0042-add-support-to-offload-TWT-Teardown-request-handling.patch \
    file://murata/0043-Add-support-to-configure-TWT-of-a-session-using-offs.patch \
    file://murata/0044-Establish-a-Default-TWT-session-in-the-STA-after-ass.patch \
    file://murata/0045-validate-the-TWT-parameters-exponent-and-mantissa-pa.patch \
    file://murata/0046-Fix-for-station-sending-open-auth-instead-of-SAE-aut.patch \
    file://murata/0047-Fix-associating-failed-when-PMK-lifetime-is-set-to-1.patch \
    file://murata/0048-non-upstream-p2p_add_group-command-unification.patch \
    file://murata/0049-non-upstream-MBO-wpa_cli-mbo-command-by-IFX-vendorID.patch \
    file://murata/0050-EAP-TLS-Allow-TLSv1.3-support-to-be-enabled-with-bui.patch \
    file://murata/0051-Enabling-TLS-v1.3-by-default.patch \
    file://murata/0052-Disable-4-way-handshake-offload-for-DPP.patch \
    file://murata/0053-non-upstream-WNM-wpa_cli-wnm_maxilde-command-by-IFX-.patch \
    file://murata/0054-brcmfmac-sync-content-of-nl80211_copy.h-for-BSS_COLO.patch \
    file://murata/0055-WPA3-SAE-enable-nl_connect-socket-while-WPA_DRIVER_F.patch \
    file://murata/0056-OWE-AP-enable-OWE-compile-option-for-hostapd-executi.patch \
    file://murata/0057-DPP2.0-support-DPP2.0-and-add-pfs-init-flow-on-EVENT.patch \
    file://murata/0058-non-upstream-Prevent-invalid-akm-key-mgmt-when-MFP-r.patch \
    file://murata/0059-Reset-authentication-and-encryption-parameters-while.patch \
    file://murata/0060-brcmfmac-add-a-configurable-link_loss-parameter-for-.patch \
"

SRC_URI:append:ccimx6sbc = " file://wpa_supplicant_p2p.conf_atheros"
SRC_URI:append:ccmp1 = " ${MURATA_COMMON_PATCHES}"

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
