# Copyright (C) 2017-2022 Digi International Inc.

SUMMARY = "Digi APIX library"
DESCRIPTION = "C library to access and manage your ConnectCore platform interfaces in an easy manner"
SECTION = "libs"
LICENSE = "ISC"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/ISC;md5=f3b90e78ea0cffb20bf5cca7947a896d"

SRCBRANCH ?= "dey-4.0/maint"
SRCREV = "9ae2f513d2ccc88b4038547709c673e86996068e"

LIBDIGIAPIX_URI_STASH = "${DIGI_MTK_GIT}/dey/libdigiapix.git;protocol=ssh"
LIBDIGIAPIX_URI_GITHUB = "${DIGI_GITHUB_GIT}/libdigiapix.git;protocol=https"

LIBDIGIAPIX_GIT_URI ?= "${@oe.utils.conditional('DIGI_INTERNAL_GIT', '1' , '${LIBDIGIAPIX_URI_STASH}', '${LIBDIGIAPIX_URI_GITHUB}', d)}"

SRC_URI = " \
    ${LIBDIGIAPIX_GIT_URI};nobranch=1 \
    file://99-digiapix.rules \
    file://libdigiapix.conf \
    file://digiapix.sh \
"

S = "${WORKDIR}/git"

inherit pkgconfig useradd python3native

DEPENDS = " \
    libsoc \
    libsocketcan \
    libgpiod \
    bluez5 \
    ${PYTHON_PN}-native \
    ${PYTHON_PN}-setuptools-native \
    ${PYTHON_PN}-pip-native \
"

EXTRA_OEMAKE += "PYTHON_BIN=${PYTHON}"

do_compile:append() {
	oe_runmake python-bindings
}

do_install() {
	oe_runmake 'DESTDIR=${D}' install install-python-bindings

	# Install udev rules for digiapix
	install -d ${D}${sysconfdir}/udev/rules.d ${D}${sysconfdir}/udev/scripts
	install -m 0644 ${WORKDIR}/99-digiapix.rules ${D}${sysconfdir}/udev/rules.d/
	install -m 0755 ${WORKDIR}/digiapix.sh ${D}${sysconfdir}/udev/scripts/

	# Install board config file
	install -m 0644 ${WORKDIR}/libdigiapix.conf ${D}${sysconfdir}/
}

PACKAGES += "${PN}-${PYTHON_PN}"
FILES:${PN}-${PYTHON_PN} = "${PYTHON_SITEPACKAGES_DIR}"
RDEPENDS:${PN}-${PYTHON_PN} = "${PN} ${PYTHON_PN}-core ${PYTHON_PN}-ctypes"

USERADD_PACKAGES = "${PN}"
GROUPADD_PARAM:${PN} = "-r digiapix"

PACKAGE_ARCH = "${MACHINE_ARCH}"
