# Copyright (C) 2017, Digi International Inc.

SUMMARY = "Digi's device cloud connector"
SECTION = "libs"
LICENSE = "MPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MPL-2.0;md5=815ca599c9df247a0c7f619bab123dad"

DEPENDS = "confuse openssl recovery-utils zlib"

SRCBRANCH = "master"
SRCREV = "4ab08c79a4fae8b701277dc27097c6e52be7dcf3"

CC_STASH = "gitsm://git@stash.digi.com/cc/cc_dey.git;protocol=ssh"
CC_GITHUB = "gitsm://github.com/digi-embedded/cc_dey.git;protocol=git"

CC_GIT_URI ?= "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${CC_STASH}', '${CC_GITHUB}', d)}"

SRC_URI = "${CC_GIT_URI};nobranch=1"

S = "${WORKDIR}/git"

inherit pkgconfig

do_install() {
	oe_runmake DESTDIR=${D} install
}

PACKAGES =+ "${PN}-cert"

FILES_${PN}-cert = "${sysconfdir}/ssl/certs/Digi_Int-ca-cert-public.crt"

CONFFILES_${PN} += "${sysconfdir}/cc.conf"

RDEPENDS_${PN} = "${PN}-cert"
