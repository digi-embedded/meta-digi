# Copyright (C) 2013 Digi International.

SUMMARY = "Microwindows Graphical Engine"
SECTION = "x11/wm"
LICENSE = "GPLv2"
# License path relative to S = "${WORKDIR}/${PN}-${PV}/src"
LIC_FILES_CHKSUM = "file://LICENSE;md5=537b9004889eb701c48fc1fe78d9c30e"

PR = "${DISTRO}.r0"

SRC_URI = " \
    ftp://ftp.microwindows.org/pub/microwindows/microwindows-src-${PV}.tar.gz \
    file://0001-defconfig.patch;striplevel=2 \
"

SRC_URI[md5sum] = "901e912cf3975f6460a9bb4325557645"
SRC_URI[sha256sum] = "c0a8473842fc757ff4c225f82b83d98bba5da0dca0cf843cfc7792064a393435"

S = "${WORKDIR}/${PN}-${PV}/src"

EXTRA_OEMAKE = " \
    ARMTOOLSPREFIX=${TARGET_PREFIX} \
    INSTALL_OWNER1= \
    INSTALL_OWNER2= \
    INSTALL_PREFIX=${D}${prefix} \
"

do_install() {
	oe_runmake install
}

ALLOW_EMPTY_${PN} = "1"
