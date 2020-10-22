# Copyright (C) 2017-2020 Digi International Inc.

SUMMARY = "Digi APIX library"
DESCRIPTION = "C library to access and manage your ConnectCore platform interfaces in an easy manner"
SECTION = "libs"
LICENSE = "ISC"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/ISC;md5=f3b90e78ea0cffb20bf5cca7947a896d"

DEPENDS = "libsoc libsocketcan libgpiod"

SRCBRANCH ?= "dey-3.0/maint"
SRCREV = "bce0bfff451c67c5387ebdaa9b5da1bde82750ca"

LIBDIGIAPIX_URI_STASH = "${DIGI_MTK_GIT}dey/libdigiapix.git;protocol=ssh"
LIBDIGIAPIX_URI_GITHUB = "${DIGI_GITHUB_GIT}/libdigiapix.git;protocol=https"

LIBDIGIAPIX_GIT_URI ?= "${@oe.utils.conditional('DIGI_INTERNAL_GIT', '1' , '${LIBDIGIAPIX_URI_STASH}', '${LIBDIGIAPIX_URI_GITHUB}', d)}"

SRC_URI = " \
    ${LIBDIGIAPIX_GIT_URI};nobranch=1 \
    file://99-digiapix.rules \
    file://libdigiapix.conf \
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
	install -m 0644 ${WORKDIR}/libdigiapix.conf ${D}${sysconfdir}/
}

USERADD_PACKAGES = "${PN}"
GROUPADD_PARAM_${PN} = "-r digiapix"

PACKAGE_ARCH = "${MACHINE_ARCH}"
