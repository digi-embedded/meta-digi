# Copyright (C) 2017-2022, Digi International Inc.

SUMMARY = "Digi's device cloud connector"
SECTION = "libs"
LICENSE = "MPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MPL-2.0;md5=815ca599c9df247a0c7f619bab123dad"

DEPENDS = "confuse libdigiapix openssl recovery-utils zlib"

SRCBRANCH = "master"
SRCREV = "7754ff4b6de513b18eaa18e5de217f9394cc345a"

CC_STASH = "gitsm://git@stash.digi.com/cc/cc_dey.git;protocol=ssh"
CC_GITHUB = "gitsm://github.com/digi-embedded/cc_dey.git;protocol=git"

CC_GIT_URI ?= "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${CC_STASH}', '${CC_GITHUB}', d)}"

SRC_URI = "${CC_GIT_URI};branch=${SRCBRANCH}"

S = "${WORKDIR}/git"

inherit pkgconfig

do_install() {
	oe_runmake DESTDIR=${D} install
}

PACKAGES =+ "${PN}-cert"

FILES_${PN}-cert = "${sysconfdir}/ssl/certs/Digi_Int-ca-cert-public.crt"

CONFFILES_${PN} += "${sysconfdir}/cc.conf"

RDEPENDS_${PN} = "${PN}-cert"
