# Copyright (C) 2017 Digi International Inc.

SUMMARY = "Digi APIX library"
DESCRIPTION = "C library to access and manage your ConnectCore platform interfaces in an easy manner"
SECTION = "libs"
LICENSE = "ISC"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/ISC;md5=f3b90e78ea0cffb20bf5cca7947a896d"

DEPENDS = "libsoc libsocketcan"

SRCBRANCH ?= "dey-2.4/maint"
SRCREV = "dfd6fdec661d1c06bd3a62f6d80fbd43a806b628"

LIBDIGIAPIX_URI_STASH = "${DIGI_MTK_GIT}dey/libdigiapix.git;protocol=ssh"
LIBDIGIAPIX_URI_GITHUB = "git://github.com/digi-embedded/libdigiapix.git;protocol=git"

LIBDIGIAPIX_GIT_URI ?= "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${LIBDIGIAPIX_URI_STASH}', '${LIBDIGIAPIX_URI_GITHUB}', d)}"

SRC_URI = " \
    ${LIBDIGIAPIX_GIT_URI};nobranch=1 \
    file://99-digiapix.rules \
    file://board.conf \
    file://digiapix.sh \
"

S = "${WORKDIR}/git"

inherit pkgconfig useradd

do_install() {
	oe_runmake 'DESTDIR=${D}' install

	# Install udev rules for digiapix
	install -d ${D}${sysconfdir}/udev/rules.d ${D}${sysconfdir}/udev/scripts
	install -m 0644 ${WORKDIR}/99-digiapix.rules ${D}${sysconfdir}/udev/rules.d/
	install -m 0755 ${WORKDIR}/digiapix.sh ${D}${sysconfdir}/udev/scripts/

	# Install board config file
	install -m 0644 ${WORKDIR}/board.conf ${D}${sysconfdir}/libdigiapix.conf
}

USERADD_PACKAGES = "${PN}"
GROUPADD_PARAM_${PN} = "-r digiapix"

PACKAGE_ARCH = "${MACHINE_ARCH}"
