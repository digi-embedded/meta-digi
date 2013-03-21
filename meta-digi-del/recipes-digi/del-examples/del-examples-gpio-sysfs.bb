# Copyright (C) 2013 Digi International.

DESCRIPTION = "DEL examples: GPIO sysfs API test application"
SECTION = "examples"
LICENSE = "GPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

PR = "r0"

SRC_URI = "file://gpio_sysfs_test"

S = "${WORKDIR}/gpio_sysfs_test"

do_compile() {
	${CC} -O2 -Wall gpio_sysfs_test.c sysfsgpio.c -o gpio_sysfs_test
}

do_install() {
	install -d ${D}${bindir}
	install -m 0755 gpio_sysfs_test ${D}${bindir}
}

PACKAGE_ARCH = "${MACHINE_ARCH}"
