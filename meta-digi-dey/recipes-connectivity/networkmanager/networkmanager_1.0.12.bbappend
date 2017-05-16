# Copyright (C) 2017, Digi International Inc.

FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

SRC_URI += " \
    file://NetworkManager.conf \
    file://networkmanager-init \
"

PACKAGECONFIG_remove = "dnsmasq netconfig"
PACKAGECONFIG_append = " concheck modemmanager ppp"

inherit update-rc.d

do_install_append() {
	install -d ${D}${sysconfdir}/init.d ${D}${sysconfdir}/NetworkManager
	install -m 0644 ${WORKDIR}/NetworkManager.conf ${D}${sysconfdir}/NetworkManager/
	install -m 0755 ${WORKDIR}/networkmanager-init ${D}${sysconfdir}/init.d/networkmanager
}

# NetworkManager needs to be started after DBUS
INITSCRIPT_NAME = "networkmanager"
INITSCRIPT_PARAMS = "start 03 2 3 4 5 . stop 80 0 6 1 ."
