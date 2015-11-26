# Copyright (C) 2015 Digi International.

FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"
SRC_URI += "file://bluez-init"

inherit update-rc.d

PACKAGECONFIG_append = " experimental"

do_install_append() {
	install -d ${D}${sbindir} ${D}${sysconfdir}/init.d/
	install -m 0755 ${WORKDIR}/bluez-init ${D}${sysconfdir}/init.d/bluez
	# gatttool is useful for BLE work but not installed by default
	install -m 0755 attrib/gatttool ${D}${sbindir}/gatttool
}

INITSCRIPT_NAME = "bluez"
INITSCRIPT_PARAMS = "start 10 5 ."
