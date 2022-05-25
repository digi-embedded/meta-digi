# Copyright (C) 2013-2022, Digi International Inc.

SUMMARY = "DEY examples: application to perform low level bluetooth"
SECTION = "examples"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"

DEPENDS = "bluez5"

SRC_URI = "file://btconfig"

S = "${WORKDIR}/btconfig"

do_compile() {
	${CC} -O2 -Wall ${LDFLAGS} btconfig.c -o btconfig -lbluetooth
}

do_install() {
	install -d ${D}${bindir}
	install -m 0755 btconfig ${D}${bindir}
}
