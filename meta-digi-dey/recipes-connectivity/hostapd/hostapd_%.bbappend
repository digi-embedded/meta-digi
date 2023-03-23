# Copyright (C) 2016-2023 Digi International.

FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

SRC_URI:append = " \
    file://hostapd_wlan0.conf \
    file://hostapd@.service \
    ${@oe.utils.conditional('HAS_WIFI_VIRTWLANS', 'true', 'file://hostapd_wlan1.conf', '', d)} \
"

SRC_URI:append:ccimx93 = " \
    file://hostapd_uap0.conf \
"

# Patch series from Murata release
MURATA_COMMON_PATCHES = " \
	file://murata/0003-nl80211-Report-connection-authorized-in-EVENT_ASSOC.patch \
	file://murata/0005-OpenSSL-Fix-build-with-OpenSSL-1.0.1.patch \
	file://murata/0006-nl80211-Check-SAE-authentication-offload-support.patch \
	file://murata/0007-SAE-Pass-SAE-password-on-connect-for-SAE-authenticat.murata.patch \
	file://murata/0008-nl80211-Support-4-way-handshake-offload-for-WPA-WPA2.patch \
	file://murata/0009-AP-Support-4-way-handshake-offload-for-WPA-WPA2-PSK.patch \
	file://murata/0010-nl80211-Support-SAE-authentication-offload-in-AP-mod.patch \
	file://murata/0011-SAE-Support-SAE-authentication-offload-in-AP-mode.patch \
	file://murata/0013-non-upstream-defconfig_base-Add-Infineon-default-con.patch \
	file://murata/0014-CVE_2019_9501-Fix-to-check-Invalid-GTK-IE-length-in-.patch \
	file://murata/0015-Add-CONFIG_WPA3_SAE_AUTH_EARLY_SET-flags-and-codes.murata.patch \
	file://murata/0016-SAE-Set-the-right-WPA-Versions-for-FT-SAE-key-manage.patch \
	file://murata/0017-wpa_supplicant-Support-WPA_KEY_MGMT_FT-for-eapol-off.murata.patch \
	file://murata/0018-wpa_supplicant-suppress-deauth-for-PMKSA-caching-dis.murata.patch \
	file://murata/0019-Fix-for-PMK-expiration-issue-through-supplicant.murata.patch \
	file://murata/0021-Avoid-deauthenticating-STA-if-the-reason-for-freeing.patch \
	file://murata/0022-wpa_supplicant-support-bgscan.patch \
	file://murata/0023-non-upstream-wl-cmd-create-interface-to-support-driv.murata.patch \
	file://murata/0024-non-upstream-wl-cmd-create-wl_do_cmd-as-an-entry-doi.patch \
	file://murata/0025-non-upstream-wl-cmd-create-ops-table-to-do-wl-comman.patch \
	file://murata/0026-non-upstream-wl-cmd-add-more-compile-flag.murata.patch \
	file://murata/0027-Fix-dpp-config-parameter-setting.patch \
	file://murata/0028-DPP-Resolving-failure-of-dpp-configurator-exchange-f.patch \
	file://murata/0029-Enabling-SUITEB192-and-SUITEB-compile-options.patch \
	file://murata/0030-DPP-Enabling-CLI_EDIT-option-for-enrollee-plus-respo.patch \
	file://murata/0032-non-upstream-SAE-disconnect-after-PMKSA-cache-expire.patch \
	file://murata/0034-wpa_supplicant-Set-PMKSA-to-driver-while-key-mgmt-is.patch \
	file://murata/0035-nl80211-Set-NL80211_SCAN_FLAG_COLOCATED_6GHZ-in-scan.murata.patch \
	file://murata/0037-Enabling-OWE-in-wpa_supplicant.patch \
	file://murata/0039-FT-Sync-nl80211-ext-feature-index.patch \
	file://murata/0040-nl80211-Introduce-a-vendor-header-for-vendor-NL-ifac.patch \
	file://murata/0041-add-support-to-offload-TWT-setup-request-handling-to.murata.patch \
	file://murata/0042-add-support-to-offload-TWT-Teardown-request-handling.murata.patch \
	file://murata/0043-Add-support-to-configure-TWT-of-a-session-using-offs.murata.patch \
	file://murata/0048-Fix-associating-failed-when-PMK-lifetime-is-set-to-1.patch \
"

SRC_URI:append:ccmp1 = " ${MURATA_COMMON_PATCHES}"

SYSTEMD_SERVICE:${PN}:append = " hostapd@.service"

do_install:append() {
	# Remove the default hostapd.conf
	rm -f ${D}${sysconfdir}/hostapd.conf

	# Install custom hostapd_IFACE.conf files
	add_hostapd_files

	# Install interface-specific systemd service
	install -m 0644 ${WORKDIR}/hostapd@.service ${D}${systemd_unitdir}/system/
	sed -i -e 's,@SBINDIR@,${sbindir},g' -e 's,@SYSCONFDIR@,${sysconfdir},g' ${D}${systemd_unitdir}/system/hostapd@.service

	# Read-only rootfs: actions that substitute postinst script
	# - append the ${DIGI_FAMILY} string to SSID
	if [ -n "${@bb.utils.contains('IMAGE_FEATURES', 'read-only-rootfs', '1', '', d)}" ]; then
		sed -i -e "s,##MAC##,${DIGI_FAMILY},g" ${D}${sysconfdir}/hostapd_*.conf
	fi
}

add_hostapd_files() {
	install -m 0644 ${WORKDIR}/hostapd_wlan0.conf ${D}${sysconfdir}

	if ${HAS_WIFI_VIRTWLANS}; then
		# Install custom hostapd_IFACE.conf file
		install -m 0644 ${WORKDIR}/hostapd_wlan1.conf ${D}${sysconfdir}
	fi
}

add_hostapd_files:ccimx93() {
	install -m 0644 ${WORKDIR}/hostapd_uap0.conf ${D}${sysconfdir}
}

pkg_postinst_ontarget:${PN}() {
	# Exit if there is no wireless hardware available
	if [ ! -e /proc/device-tree/wireless/mac-address ]; then
		exit 0
	fi

	# Append the last two bytes of the wlan0 MAC address to the SSID of the
	# hostAP configuration files

	# Get the last two bytes of the wlan0 MAC address
	MAC="$(dd conv=swab if=/proc/device-tree/wireless/mac-address 2>/dev/null | hexdump | head -n 1 | cut -d ' ' -f 4)"

	find "${sysconfdir}" -type f -name 'hostapd_*.conf' -exec \
		sed -i -e "s,##MAC##,${MAC},g" {} \;

	# Do not autostart hostapd daemon, it will conflict with wpa-supplicant.
	if type update-rc.d >/dev/null 2>/dev/null; then
		# Remove all symlinks in the different runlevels
		update-rc.d -f ${INITSCRIPT_NAME} remove
	fi
}

inherit ${@bb.utils.contains("IMAGE_FEATURES", "read-only-rootfs", "remove-pkg-postinst-ontarget", "", d)}
