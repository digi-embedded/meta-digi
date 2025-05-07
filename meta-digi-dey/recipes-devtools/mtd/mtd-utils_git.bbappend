# Copyright (C) 2023-2025, Digi International Inc.

PACKAGECONFIG:append = " crypto"
FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SRC_URI += " \
    file://ubihealthd-init \
    file://ubihealthd.service \
"

inherit systemd update-rc.d

do_install:append:class-target() {
	install -d ${D}${sysconfdir}/init.d/
	install -m 0755 ${WORKDIR}/ubihealthd-init ${D}${sysconfdir}/
	ln -sf ${sysconfdir}/ubihealthd-init ${D}${sysconfdir}/init.d/ubihealthd-init

	install -d ${D}${systemd_unitdir}/system/
	install -m 0644 ${WORKDIR}/ubihealthd.service ${D}${systemd_unitdir}/system/
}

FILES:mtd-utils-ubifs += " \
    ${sysconfdir}/ubihealthd-init \
    ${sysconfdir}/init.d/ubihealthd-init \
    ${systemd_unitdir}/system/ubihealthd.service \
"

INITSCRIPT_PACKAGES += "mtd-utils-ubifs"
INITSCRIPT_NAME:mtd-utils-ubifs = "ubihealthd-init"
INITSCRIPT_PARAMS:mtd-utils-ubifs = "start 19 2 3 4 5 . stop 21 0 1 6 ."

SYSTEMD_PACKAGES = "mtd-utils-ubifs"
SYSTEMD_SERVICE:mtd-utils-ubifs = "ubihealthd.service"
