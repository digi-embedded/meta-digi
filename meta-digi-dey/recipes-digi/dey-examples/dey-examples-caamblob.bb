# Copyright (C) 2017-2022, Digi International Inc.

require recipes-digi/dey-examples/dey-examples-src.inc

SUMMARY = "DEY examples: CAAM blob example application"
SECTION = "examples"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"

S = "${WORKDIR}/git/caam-blob-example"

inherit pkgconfig

do_install() {
	oe_runmake DESTDIR=${D} install
}
