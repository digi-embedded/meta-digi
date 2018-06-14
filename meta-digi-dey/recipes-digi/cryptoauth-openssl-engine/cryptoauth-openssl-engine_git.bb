# Copyright (C) 2018 Digi International Inc.

SUMMARY = "Microchip CryptoAuthentication OpenSSL engine"
SECTION = "libs"
LICENSE = "MICROCHIP_ENGINE_LICENSE"
LIC_FILES_CHKSUM = "file://LICENSE;md5=3fdaa96f37898a0641820700bbf5f7b8"

SRCBRANCH = "master"
SRCREV = "a69a4f92af6bee9cb13035c2f859912744796380"

GIT_URI ?= "git://github.com/MicrochipTech/cryptoauth-openssl-engine.git;protocol=git"

SRC_URI = " \
    ${GIT_URI};nobranch=1 \
    file://0001-Digi-modifications-to-the-cryptoauth-OpenSSL-engine.patch \
"

S = "${WORKDIR}/git"

I2C_BUS ?= "0"
I2C_BUS_ccimx6qpsbc = "1"

I2C_SPEED ?= "100000"

CFLAGS += "-DATCA_HAL_I2C_BUS=${I2C_BUS} -DATCA_HAL_I2C_SPEED=${I2C_SPEED}"

do_install() {
	oe_runmake DESTDIR=${D} install
}

DEPENDS += "openssl"

TARGET_CC_ARCH += "${LDFLAGS}"
FILES_${PN} += "${libdir}/ssl/engines/libateccssl.so"

PACKAGE_ARCH = "${MACHINE_ARCH}"
COMPATIBLE_MACHINE = "(ccimx6qpsbc|ccimx6ul|ccimx8x)"
