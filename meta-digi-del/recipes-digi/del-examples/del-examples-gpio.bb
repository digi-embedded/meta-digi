# Copyright (C) 2013 Digi International.

DESCRIPTION = "DEL examples: GPIO kernel module test application"
SECTION = "examples"
LICENSE = "GPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

PR = "${DISTRO}.r0"

RDEPENDS_${PN} += "kernel-module-gpio"

SRC_URI = "file://gpio_test"

S = "${WORKDIR}/gpio_test"

do_compile() {
	${CC} -O2 -Wall gpio_test.c -o gpio_test
}

do_install() {
	install -d ${D}${bindir}
	install -m 0755 gpio_test ${D}${bindir}
}

PACKAGE_ARCH = "${MACHINE_ARCH}"
