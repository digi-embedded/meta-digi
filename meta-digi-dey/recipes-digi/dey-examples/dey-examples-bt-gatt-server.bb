# Copyright (C) 2019, Digi International Inc.

require recipes-digi/dey-examples/dey-examples-src.inc

SUMMARY = "DEY examples: application to create a BLE GATT server"
SECTION = "examples"
LICENSE = "GPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

DEPENDS = "bluez5"

S = "${WORKDIR}/git/ble-gatt-server-example"

do_install() {
	oe_runmake DESTDIR=${D} install
}
