# Copyright (C) 2016-2020 Digi International.

FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

SRC_URI_append = " \
    file://hostapd_wlan0.conf \
    file://hostapd@.service \
    ${@oe.utils.conditional('HAS_WIFI_VIRTWLANS', 'true', 'file://hostapd_wlan1.conf', '', d)} \
"

SYSTEMD_SERVICE_${PN}_append = " hostapd@.service"

do_install_append() {
	# Remove the default hostapd.conf
	rm -f ${WORKDIR}/hostapd.conf
	# Install custom hostapd_IFACE.conf file
	install -m 0644 ${WORKDIR}/hostapd_wlan0.conf ${D}${sysconfdir}
	# Install interface-specific systemd service
	install -m 0644 ${WORKDIR}/hostapd@.service ${D}${systemd_unitdir}/system/
	sed -i -e 's,@SBINDIR@,${sbindir},g' -e 's,@SYSCONFDIR@,${sysconfdir},g' ${D}${systemd_unitdir}/system/hostapd@.service

	if ${HAS_WIFI_VIRTWLANS}; then
		# Install custom hostapd_IFACE.conf file
		install -m 0644 ${WORKDIR}/hostapd_wlan1.conf ${D}${sysconfdir}
	fi
}

pkg_postinst_ontarget_${PN}() {
	# Exit if there is no wireless hardware available
	if [ ! -e /proc/device-tree/wireless/mac-address ]; then
		exit 0
	fi

	# Append the last two bytes of the wlan0 MAC address to the SSID of the
	# hostAP configuration files

	# Get the last two bytes of the wlan0 MAC address
	MAC="$(dd conv=swab if=/proc/device-tree/wireless/mac-address 2>/dev/null | hexdump | head -n 1 | cut -d ' ' -f 4)"

	find "${sysconfdir}" -type f -name 'hostapd_wlan?.conf' -exec \
		sed -i -e "s,##MAC##,${MAC},g" {} \;

	# Do not autostart hostapd daemon, it will conflict with wpa-supplicant.
	if type update-rc.d >/dev/null 2>/dev/null; then
		# Remove all symlinks in the different runlevels
		update-rc.d -f ${INITSCRIPT_NAME} remove
	fi
}

inherit ${@bb.utils.contains("IMAGE_FEATURES", "read-only-rootfs", "remove-pkg-postinst-ontarget", "", d)}
