# Copyright (C) 2013 Digi International.

SUMMARY = "DEY examples: accelerometer test application"
SECTION = "examples"
LICENSE = "GPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

RDEPENDS_${PN} = "kernel-module-mma7455l"

SRC_URI = "file://accelerometer_test"

inherit update-rc.d

S = "${WORKDIR}/accelerometer_test"

do_compile() {
	${CC} -O2 -Wall accelerometer_test.c -o accelerometer_test
}

do_install() {
	install -d ${D}${bindir} ${D}${sysconfdir}/init.d/
	install -m 0755 accelerometer_test accelerometer_calibrate ${D}${bindir}
	install -m 0755 accelerometer_init ${D}${sysconfdir}/init.d/accelerometer
}

INITSCRIPT_NAME = "accelerometer"
INITSCRIPT_PARAMS = "start 11 S ."

PACKAGE_ARCH = "${MACHINE_ARCH}"
