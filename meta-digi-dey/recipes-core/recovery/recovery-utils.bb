# Copyright (C) 2017 Digi International

SUMMARY = "Recovery reboot utilities"
LICENSE = "MPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MPL-2.0;md5=815ca599c9df247a0c7f619bab123dad"

DEPENDS = "libubootenv"

PV = "0.1"

SRC_URI = "file://${BPN}"

S = "${WORKDIR}/${BPN}"

do_install() {
	oe_runmake DESTDIR=${D} install
}
