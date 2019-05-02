# Copyright (C) 2017, 2018 Digi International.

SUMMARY = "DEY examples: Cryptochip example application"
SECTION = "examples"
LICENSE = "GPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

DEPENDS = "cryptoauthlib"

SRCBRANCH = "dey-2.4/maint"
SRCREV = "3f507e3116fe374e2ce2e670acc0c9d95dcff4ff"

CC_STASH = "${DIGI_MTK_GIT}dey/dey-examples.git;protocol=ssh"
CC_GITHUB = "${DIGI_GITHUB_GIT}/dey-examples.git;protocol=git"

CC_GIT_URI ?= "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${CC_STASH}', '${CC_GITHUB}', d)}"

SRC_URI = "${CC_GIT_URI};nobranch=1"

S = "${WORKDIR}/git/cryptochip-get-random"

inherit pkgconfig

do_install() {
	oe_runmake DESTDIR=${D} install
}

PACKAGE_ARCH = "${MACHINE_ARCH}"
COMPATIBLE_MACHINE = "(ccimx6qpsbc|ccimx6ul|ccimx8x)"
