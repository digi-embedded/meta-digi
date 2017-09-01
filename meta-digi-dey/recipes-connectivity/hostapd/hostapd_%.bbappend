# Copyright (C) 2016,2017 Digi International.

FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

SRC_URI_append = " file://hostapd_wlan0.conf"
SRC_URI_append_ccimx6ul = " file://hostapd_wlan1.conf"

do_install_append() {
	# Remove the default hostapd.conf
	rm -f ${WORKDIR}/hostapd.conf
	# Install custom hostapd_IFACE.conf file
	install -m 0644 ${WORKDIR}/hostapd_wlan0.conf ${D}${sysconfdir}
}

do_install_append_ccimx6ul() {
	# Install custom hostapd_IFACE.conf file
	install -m 0644 ${WORKDIR}/hostapd_wlan1.conf ${D}${sysconfdir}
}

pkg_postinst_${PN}() {
	# Append the last two bytes of the wlan0 MAC address to the SSID of the
	# hostAP configuration files
	# (execute on first boot)
	if [ -n "$D" ]; then
		exit 1
	fi

	# Get the last two bytes of the wlan0 MAC address
	MAC="$(cut -d ':' -f5,6 /sys/class/net/wlan0/address | tr -d ':')"

	# If wlan0 is not available, use a random value with no hexadecimal characters
	if [ -z "${MAC}" ]; then
		MAC="$(cat /dev/urandom | tr -dc 'G-Z' | fold -w 4 | head -n 1)"
	fi

	find "${sysconfdir}" -type f -name 'hostapd_wlan?.conf' -exec \
		sed -i -e "s,##MAC##,${MAC},g" {} \;

	# Create the symlinks in the different runlevels
	if type update-rc.d >/dev/null 2>/dev/null; then
		update-rc.d ${INITSCRIPT_NAME} ${INITSCRIPT_PARAMS}
	fi
}

# Do not autostart hostapd daemon, it will conflict with wpa-supplicant.
INITSCRIPT_PARAMS = "remove"
