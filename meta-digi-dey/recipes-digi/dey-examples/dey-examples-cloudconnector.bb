# Copyright (C) 2017, 2018 Digi International Inc.

SUMMARY = "DEY examples: Remote Manager test applications"
SECTION = "examples"
LICENSE = "MPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MPL-2.0;md5=815ca599c9df247a0c7f619bab123dad"

DEPENDS = "cloudconnector"

SRC_URI = "file://cloudconnector_test"

S = "${WORKDIR}/cloudconnector_test"

inherit pkgconfig

do_install() {
	oe_runmake DESTDIR=${D} install
}

RDEPENDS_${PN} = "cloudconnector-cert"
