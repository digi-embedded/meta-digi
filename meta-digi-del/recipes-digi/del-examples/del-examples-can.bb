# Copyright (C) 2013 Digi International.

DESCRIPTION = "DEL examples: CAN bus test application"
SECTION = "examples"
LICENSE = "GPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

PR = "r0"

SRC_URI = "file://can_test"

S = "${WORKDIR}/can_test"

do_compile() {
	${CC} -O2 -Wall can_test.c -o can_test -pthread
}

do_install() {
	install -d ${D}${bindir}
	install -m 0755 can_test ${D}${bindir}
}

PACKAGE_ARCH = "${MACHINE_ARCH}"
