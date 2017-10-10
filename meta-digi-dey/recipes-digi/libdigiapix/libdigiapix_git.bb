# Copyright (C) 2017 Digi International Inc.

SUMMARY = "Digi APIX library"
DESCRIPTION = "C library to access and manage your ConnectCore platform interfaces in an easy manner"
SECTION = "libs"
LICENSE = "ISC"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/ISC;md5=f3b90e78ea0cffb20bf5cca7947a896d"

DEPENDS = "libsoc"

SRCBRANCH ?= "master"
SRCREV = "${AUTOREV}"

LIBDIGIAPIX_URI_STASH = "${DIGI_MTK_GIT}dey/libdigiapix.git;protocol=ssh"
LIBDIGIAPIX_URI_GITHUB = "git://github.com/digi-embedded/libdigiapix.git;protocol=git"

LIBDIGIAPIX_GIT_URI ?= "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${LIBDIGIAPIX_URI_STASH}', '${LIBDIGIAPIX_URI_GITHUB}', d)}"

SRC_URI = " \
    ${LIBDIGIAPIX_GIT_URI};branch=${SRCBRANCH} \
    file://board.conf \
"

S = "${WORKDIR}/git"

inherit pkgconfig

do_install() {
	oe_runmake 'DESTDIR=${D}' install

	install -d ${D}${sysconfdir}/
	install -m 0644 ${WORKDIR}/board.conf ${D}${sysconfdir}/libdigiapix.conf
}

PACKAGE_ARCH = "${MACHINE_ARCH}"
