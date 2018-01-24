# Copyright (C) 2015-2018 Digi International.

FILESEXTRAPATHS_prepend := "${THISDIR}/${BP}:"

SRC_URI += " \
    file://bluetooth-init \
    file://main.conf \
    file://0001-hcitool-do-not-show-unsupported-refresh-option.patch \
    file://0002-hcitool-increase-the-shown-connection-limit-to-20.patch \
    file://0003-port-test-discovery-to-python3.patch \
"

QCA6564_COMMON_PATCHES = " \
    file://0004-QCA_bluetooth_chip_support.patch \
    file://0005-hciattach_rome-Respect-the-user-indication-for-noflo.patch \
    file://0006-hciattach-If-the-user-supplies-a-bdaddr-use-it.patch \
    file://0007-hciattach-Add-verbosity-option.patch \
"

SRC_URI_append_ccimx6ul = " ${QCA6564_COMMON_PATCHES}"
SRC_URI_append_ccimx6qpsbc = " ${QCA6564_COMMON_PATCHES}"

inherit update-rc.d

do_install_append() {
	install -d ${D}${sysconfdir}/init.d/
	install -m 0755 ${WORKDIR}/bluetooth-init ${D}${sysconfdir}/init.d/bluetooth-init
	install -m 0644 ${WORKDIR}/main.conf ${D}${sysconfdir}/bluetooth/
}

PACKAGES =+ "${PN}-init"

FILES_${PN} += " ${sysconfdir}/bluetooth/main.conf"
FILES_${PN}-init = "${sysconfdir}/init.d/bluetooth-init"

INITSCRIPT_PACKAGES += "${PN}-init"
INITSCRIPT_NAME_${PN}-init = "bluetooth-init"
INITSCRIPT_PARAMS_${PN}-init = "start 19 2 3 4 5 . stop 21 0 1 6 ."

PACKAGE_ARCH = "${MACHINE_ARCH}"
