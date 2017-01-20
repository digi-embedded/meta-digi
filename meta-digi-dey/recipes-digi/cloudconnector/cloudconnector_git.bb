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

do_install() {
	oe_runmake DESTDIR=${D} install
}

PACKAGES =+ "${PN}-bin ${PN}-cert"

FILES_${PN}-bin += "${sysconfdir}/cc.conf"
FILES_${PN}-cert = "${sysconfdir}/ssl/certs/Digi_Int-ca-cert-public.crt"

CONFFILES_${PN}-bin += "${sysconfdir}/cc.conf"

DEBIAN_NOAUTONAME_${PN}-bin = "1"
DEBIAN_NOAUTONAME_${PN}-cert = "1"

RDEPENDS_${PN} = "${PN}-cert"
RDEPENDS_${PN}-bin = "${PN}-cert"
