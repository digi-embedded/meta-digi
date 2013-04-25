# Copyright (C) 2013 Digi International.

SUMMARY = "Digi's memory access utility"
SECTION = "base"
LICENSE = "GPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

PR = "${DISTRO}.r0"

SRC_URI = "file://memwatch.c \
           "

S = "${WORKDIR}"

do_compile() {
	${CC} -O2 -Wall -o memwatch memwatch.c
}

do_install() {
	install -d ${D}${bindir}
	install -m 0755 memwatch ${D}${bindir}
}
