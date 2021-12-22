# Copyright (C) 2017-2020, Digi International Inc.

require recipes-digi/dey-examples/dey-examples-src.inc

SUMMARY = "DEY examples: Cryptochip example application"
SECTION = "examples"
LICENSE = "GPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"

DEPENDS = "cryptoauthlib"

S = "${WORKDIR}/git/cryptochip-get-random"

inherit pkgconfig

do_install() {
	oe_runmake DESTDIR=${D} install
}

PACKAGE_ARCH = "${MACHINE_ARCH}"
COMPATIBLE_MACHINE = "(ccimx6qpsbc|ccimx6ul|ccimx8x|ccimx8m)"
