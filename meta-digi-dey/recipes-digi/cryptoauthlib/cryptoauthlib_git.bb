# Copyright (C) 2017, Digi International Inc.

SUMMARY = "Atmel CryptoAuthentication Library"
SECTION = "libs"
LICENSE = "ATMEL_CRYPTOAUTHLIB_LICENSE"
LIC_FILES_CHKSUM = "file://lib/atca_cfgs.h;beginline=8;endline=40;md5=073d05cb7a4312aaff0af9186e4fa93e"

SRCBRANCH = "dey-2.2/maint"
SRCREV = "${AUTOREV}"

GIT_URI ?= "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${DIGI_MTK_GIT}/linux/atmel-cryptoauth-openssl-engine.git;protocol=ssh', '${DIGI_GITHUB_GIT}/cryptoauth-openssl-engine.git', d)}"

SRC_URI = "${GIT_URI};branch=${SRCBRANCH}"

S = "${WORKDIR}/git/engine_atecc/cryptoauthlib"

I2C_BUS = ""
I2C_BUS_ccimx6qpsbc = "1"
I2C_BUS_ccimx6ul = "0"

I2C_SPEED = ""
I2C_SPEED_ccimx6qpsbc = "100000"
I2C_SPEED_ccimx6ul = "100000"

CFLAGS += "-DATCA_HAL_I2C_BUS=${I2C_BUS} -DATCA_HAL_I2C_SPEED=${I2C_SPEED}"

do_install() {
	oe_runmake DESTDIR=${D} install
}

PACKAGE_ARCH = "${MACHINE_ARCH}"
COMPATIBLE_MACHINE = "(ccimx6qpsbc|ccimx6ul)"
