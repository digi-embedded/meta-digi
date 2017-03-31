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

# Do not autostart hostapd daemon, it will conflict with wpa-supplicant.
INITSCRIPT_PARAMS = "remove"
