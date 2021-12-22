# Copyright (C) 2013,2017 Digi International.

SUMMARY = "DEY examples: RTC test application"
SECTION = "examples"
LICENSE = "GPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"

SRC_URI = "file://rtc_test"

S = "${WORKDIR}/rtc_test"

do_compile() {
	${CC} -O2 -Wall ${LDFLAGS} rtc_test.c -o rtc_test
}

do_install() {
	install -d ${D}${bindir}
	install -m 0755 rtc_test ${D}${bindir}
}
