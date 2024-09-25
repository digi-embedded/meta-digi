# Copyright (C) 2017-2023, Digi International Inc.

SUMMARY = "Microchip CryptoAuthentication Library"
SECTION = "libs"
LICENSE = "MICROCHIP_CRYPTOAUTHLIB_LICENSE"
LIC_FILES_CHKSUM = "file://license.txt;md5=84f2905dc39d2f8cdffb00af6f9e6d4e"

SRCREV = "a0007d2f6c42fddab5dca1575e0f404788829ddc"

GIT_URI ?= "git://github.com/digi-embedded/cryptoauthlib.git;protocol=https;nobranch=1"

SRC_URI = " \
    ${GIT_URI} \
    file://0001-lib-add-parameters-to-be-able-to-modify-default-I2C-.patch \
    file://0002-lib-apply-library-version-number-to-CMake-VERSION-pr.patch \
    file://0003-pkcs11-rename-template-configuration-file-to-its-int.patch \
    file://0004-lib-install-pkg-config-file-and-header-files.patch \
    file://0005-test-add-CMakeLists.txt.patch \
    file://0006-Remove-unnecessary-code-from-cryptoauth_test.patch \
    file://0007-lib-set-ATECC508A-as-default-device-type-in-default.patch \
"

S = "${WORKDIR}/git"

I2C_BUS = ""
I2C_BUS:ccimx6qpsbc = "1"
I2C_BUS:ccimx6ul = "0"
I2C_BUS:ccimx8x = "0"
I2C_BUS:ccimx8m = "0"

I2C_SPEED ?= "400000"
I2C_SPEED:ccimx6qpsbc = "100000"

EXTRA_OECMAKE += "-DATCA_HAL_I2C_BUS=${I2C_BUS} -DATCA_HAL_I2C_SPEED=${I2C_SPEED} -DBUILD_TESTS=on"

inherit cmake

do_install:append() {
	# Rename the folder containing the header files to be more package-specific
	mv ${D}${includedir}/lib ${D}${includedir}/cryptoauthlib

	# Remove RPATH from the executable
	chrpath -d ${D}${bindir}/cryptoauth_test
	chmod +x ${D}${bindir}/cryptoauth_test
}

PACKAGES =+ "${PN}-test"

FILES:${PN}-test = "${bindir}/cryptoauth_test"

RDEPENDS:${PN} = "libp11"
RDEPENDS:${PN}-test = "${PN}"
RRECOMMENDS:${PN} = "${PN}-test"

PACKAGE_ARCH = "${MACHINE_ARCH}"
COMPATIBLE_MACHINE = "(ccimx6qpsbc|ccimx6ul|ccimx8m|ccimx8x)"
