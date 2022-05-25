# Copyright (C) 2013-2022, Digi International Inc.

SUMMARY = "DEY examples: bluetooth health profile test application"
SECTION = "examples"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"

SRC_URI = "file://hdp_test"

S = "${WORKDIR}/hdp_test"

do_install() {
	install -d ${D}${bindir}
	install -m 0755 hdp-test.py ${D}${bindir}
}

RDEPENDS:${PN} = "python3 python3-crypt python3-dbus python3-pygobject"
