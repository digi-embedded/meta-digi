# Copyright (C) 2013,2017 Digi International.

SUMMARY = "DEY examples: ALSA API test application"
SECTION = "examples"
LICENSE = "GPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"

DEPENDS = "alsa-lib"

SRC_URI = "file://alsa_test"

S = "${WORKDIR}/alsa_test"

do_compile() {
	${CC} -O2 -Wall ${LDFLAGS} alsa_test.c -o alsa_test -lasound
}

do_install() {
	install -d ${D}${bindir}
	install -m 0755 alsa_test ${D}${bindir}
}
