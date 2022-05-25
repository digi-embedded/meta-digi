# Copyright (C) 2022, Digi International Inc.

SUMMARY = "DEY examples: Tamper test application"
SECTION = "examples"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"

SRC_URI = "file://tamper_sample"

S = "${WORKDIR}/tamper_sample"

do_install() {
	install -d ${D}${bindir}
	install -m 0755 tamper_sample ${D}${bindir}
}
