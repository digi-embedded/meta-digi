# Copyright (C) 2017, Digi International Inc.

SUMMARY = "Digi's device cloud connector"
SECTION = "libs"
LICENSE = "MPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MPL-2.0;md5=815ca599c9df247a0c7f619bab123dad"

DEPENDS = "confuse openssl recovery-utils zlib"

SRC_URI = "${DIGI_PKG_SRC}/${BP}.tar.gz"

SRC_URI[md5sum] = "db366b0358c1b47f6380080ce75d91e4"
SRC_URI[sha256sum] = "5ecd4b1830fea7746e005465b6eef30f118302147861eb4074cf717fffbdf9d5"

S = "${WORKDIR}/${PN}-${PV}"

inherit pkgconfig

do_install() {
	oe_runmake DESTDIR=${D} install
}

PACKAGES =+ "${PN}-cert"

FILES_${PN}-cert = "${sysconfdir}/ssl/certs/Digi_Int-ca-cert-public.crt"

CONFFILES_${PN} += "${sysconfdir}/cc.conf"

RDEPENDS_${PN} = "${PN}-cert"
