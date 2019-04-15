# Copyright (C) 2017-2019, Digi International Inc.

require recipes-digi/dey-examples/dey-examples-src.inc

SUMMARY = "DEY Digi APIX examples"
SECTION = "examples"
LICENSE = "ISC"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/ISC;md5=f3b90e78ea0cffb20bf5cca7947a896d"

DEPENDS = "libdigiapix"

SRC_URI = "${DEY_EXAMPLES_GIT_URI};nobranch=1"

S = "${WORKDIR}/git"

inherit pkgconfig

EXTRA_OEMAKE += "-f libdigiapix-examples.mk"

do_compile() {
	oe_runmake
}

do_install() {
	oe_runmake DESTDIR=${D} install
}
