# Copyright (C) 2013 Digi International.

SUMMARY = "DEY examples: OSS (open sound system) test application"
SECTION = "examples"
LICENSE = "GPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

PR = "${DISTRO}.r0"

SRC_URI = "file://vplay_test"

S = "${WORKDIR}/vplay_test"

do_compile() {
	${CC} -O2 -Wall vplay.c -o vplay
}

do_install() {
	install -d ${D}${bindir}
	install -m 0755 vplay ${D}${bindir}
}

PACKAGE_ARCH = "${MACHINE_ARCH}"
