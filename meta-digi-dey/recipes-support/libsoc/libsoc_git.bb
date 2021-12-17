# Copyright (C) 2017-2021 Digi International Inc.

SUMMARY = "Library for interfacing with common SoC peripherals"
DESCRIPTION = "libsoc is a C library to interface with common peripherals (gpio, i2c, spi, pwm) \
               found in SoC (System on Chips) through generic Linux Kernel interfaces."

HOMEPAGE = "https://github.com/jackmitch/libsoc"

LICENSE = "LGPLv2.1"
LIC_FILES_CHKSUM = "file://LICENCE;md5=e0bfebea12a718922225ba987b2126a5"

inherit autotools pkgconfig python3-dir

SRCBRANCH ?= "master"
SRCREV = "${AUTOREV}"
SRC_URI = "git://github.com/jackmitch/libsoc.git;branch=master;protocol=https"

S = "${WORKDIR}/git"

BOARD ??= "devboard"

PACKAGECONFIG ?= ""

PACKAGECONFIG[disabledebug] = "--disable-debug,,"
PACKAGECONFIG[allboardconfigs] = "--with-board-configs,,"
PACKAGECONFIG[enableboardconfig] = "--enable-board=${BOARD},,"
PACKAGECONFIG[python] = "--enable-python=${STAGING_BINDIR_NATIVE}/${PYTHON_PN}-native/${PYTHON_PN},,${PYTHON_PN} ${PYTHON_PN}-native"

PACKAGES =+ "${@bb.utils.contains('PACKAGECONFIG', 'python', \
    '${PYTHON_PN}-libsoc-staticdev ${PYTHON_PN}-libsoc', '', d)}"

RDEPENDS_${PN} = "libgcc"
RDEPENDS_${PYTHON_PN}-libsoc = "${PN} ${PYTHON_PN}-ctypes"

FILES_${PYTHON_PN}-libsoc-staticdev += "${PYTHON_SITEPACKAGES_DIR}/*/*.a"
FILES_${PYTHON_PN}-libsoc += "${PYTHON_SITEPACKAGES_DIR}"

DEFAULT_PREFERENCE = "-1"
