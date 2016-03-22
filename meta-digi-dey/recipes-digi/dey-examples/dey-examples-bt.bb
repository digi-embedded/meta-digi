# Copyright (C) 2013 Digi International.

SUMMARY = "DEY examples: application to transfer data over bluetooth"
SECTION = "examples"
LICENSE = "GPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

inherit bluetooth

DEPENDS = "${BLUEZ}"

SRC_URI = "file://bt_test"

S = "${WORKDIR}/bt_test"

do_compile() {
	${CC} -O2 -Wall bt_test.c -o bt_test -lbluetooth -pthread
}

do_install() {
	install -d ${D}${bindir}
	install -m 0755 bt_test ${D}${bindir}
}

COMPATIBLE_MACHINE = "(ccardimx28|ccimx6$|ccimx6ul)"
