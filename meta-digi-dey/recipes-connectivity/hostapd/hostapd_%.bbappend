# Copyright (C) 2016,2017 Digi International.

FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

SRC_URI += "file://hostapd.conf"

do_install_append() {
	# Overwrite the default hostapd.conf with our custom file
	install -m 0644 ${WORKDIR}/hostapd.conf ${D}${sysconfdir}
}

# Do not autostart hostapd daemon, it will conflict with wpa-supplicant.
INITSCRIPT_PARAMS = "remove"
