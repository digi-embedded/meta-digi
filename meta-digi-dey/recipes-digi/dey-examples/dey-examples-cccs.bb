# Copyright (C) 2017-2023, Digi International Inc.

require recipes-digi/dey-examples/dey-examples-src.inc

SUMMARY = "DEY examples: ConnectCore Cloud Services example applications"
SECTION = "examples"
LICENSE = "ISC"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/ISC;md5=f3b90e78ea0cffb20bf5cca7947a896d"

DEPENDS = "cccs"

S = "${WORKDIR}/git"

inherit pkgconfig

EXTRA_OEMAKE += "-f cccs-examples.mk"

do_compile() {
	oe_runmake
}

do_install() {
	oe_runmake DESTDIR=${D} install
}

RDEPENDS:${PN} = "cccs-daemon"
