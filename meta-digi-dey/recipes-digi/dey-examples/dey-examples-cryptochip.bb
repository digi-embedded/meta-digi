# Copyright (C) 2017 Digi International.

SUMMARY = "DEY examples: Cryptochip example application"
SECTION = "examples"
LICENSE = "GPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

DEPENDS = "cryptoauthlib"

SRCBRANCH = "dey-2.2/maint"
SRCREV = "1e02f43c812e8884509577b9f5cae77c6f1f8e08"

CC_STASH = "${DIGI_MTK_GIT}dey/dey-examples.git;protocol=ssh"
CC_GITHUB = "${DIGI_GITHUB_GIT}/dey-examples.git;protocol=git"

CC_GIT_URI ?= "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${CC_STASH}', '${CC_GITHUB}', d)}"

SRC_URI = "${CC_GIT_URI};nobranch=1"

S = "${WORKDIR}/git/cryptochip-get-random"

do_install() {
	oe_runmake DESTDIR=${D} install
}

PACKAGE_ARCH = "${MACHINE_ARCH}"
COMPATIBLE_MACHINE = "(ccimx6qpsbc|ccimx6ul)"

