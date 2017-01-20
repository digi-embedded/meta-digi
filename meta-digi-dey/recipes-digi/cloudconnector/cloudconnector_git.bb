# Copyright (C) 2017, Digi International Inc.

SUMMARY = "Digi's device cloud connector"
SECTION = "libs"
LICENSE = "MPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MPL-2.0;md5=815ca599c9df247a0c7f619bab123dad"

DEPENDS = "confuse openssl zlib"

SRCBRANCH = "master"
SRCREV = "${AUTOREV}"

SRC_URI = "gitsm://git@stash.digi.com/cc/cc_dey.git;protocol=ssh;branch=${SRCBRANCH}"

S = "${WORKDIR}/git"

inherit pkgconfig

do_install() {
	oe_runmake DESTDIR=${D} install
}

PACKAGES =+ "${PN}-cert"

FILES_${PN}-cert = "${sysconfdir}/ssl/certs/Digi_Int-ca-cert-public.crt"

CONFFILES_${PN} += "${sysconfdir}/cc.conf"

RDEPENDS_${PN} = "${PN}-cert"
