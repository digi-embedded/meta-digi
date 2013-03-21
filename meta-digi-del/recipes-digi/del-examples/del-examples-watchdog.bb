# Copyright (C) 2013 Digi International.

DESCRIPTION = "DEL examples: watchdog test application"
SECTION = "examples"
LICENSE = "GPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

PR = "r0"

SRC_URI = "file://watchdog_test"

S = "${WORKDIR}/watchdog_test"

do_compile() {
	${CC} -O2 -Wall watchdog_test.c -o watchdog_test
}

do_install() {
	install -d ${D}${bindir}
	install -m 0755 watchdog_test ${D}${bindir}
}

PACKAGE_ARCH = "${MACHINE_ARCH}"
