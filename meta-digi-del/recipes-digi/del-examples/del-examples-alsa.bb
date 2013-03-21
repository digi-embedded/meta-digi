# Copyright (C) 2013 Digi International.

DESCRIPTION = "DEL examples: ALSA API test application"
SECTION = "examples"
LICENSE = "GPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

DEPENDS = "alsa-lib"

PR = "r0"

SRC_URI = "file://alsa_test"

S = "${WORKDIR}/alsa_test"

do_compile() {
	${CC} -O2 -Wall alsa_test.c -o alsa_test -lasound
}

do_install() {
	install -d ${D}${bindir}
	install -m 0755 alsa_test ${D}${bindir}
}

PACKAGE_ARCH = "${MACHINE_ARCH}"
