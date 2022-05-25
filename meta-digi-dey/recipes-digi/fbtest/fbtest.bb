# Copyright (C) 2013-2022, Digi International Inc.

SUMMARY = "Digi's framebuffer test utility"
SECTION = "base"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"

DEPENDS = "microwindows"

SRC_URI = "file://fbtest.c"

S = "${WORKDIR}"

do_compile() {
	${CC} -O2 -Wall ${LDFLAGS} fbtest.c -o fbtest -lnano-X
}

do_install() {
	install -d ${D}${bindir}
	install -m 0755 fbtest ${D}${bindir}
}
