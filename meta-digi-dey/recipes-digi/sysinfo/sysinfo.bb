# Copyright (C) 2016 Digi International.

SUMMARY = "Digi's system info utility"
SECTION = "base"
LICENSE = "GPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

SRC_URI = "file://sysinfo"

S = "${WORKDIR}"

do_install() {
	install -d ${D}${bindir}
	install -m 0755 sysinfo ${D}${bindir}
}
