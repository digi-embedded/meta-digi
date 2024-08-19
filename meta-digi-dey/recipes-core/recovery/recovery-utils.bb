# Copyright (C) 2017-2024, Digi International Inc.

SUMMARY = "Recovery reboot utilities"
LICENSE = "MPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MPL-2.0;md5=815ca599c9df247a0c7f619bab123dad"

DEPENDS = "libubootenv"

PV = "0.1"

SRC_URI = "file://${BPN}"

S = "${WORKDIR}/${BPN}"

# Set compilation flag to disable some recovery features that are not supported
CFLAGS = "${@oe.utils.conditional('DEY_SOC_VENDOR', 'NXP', ' -DSUPPORTS_FS_ENCRYPTION', '', d)}"

do_install() {
	oe_runmake DESTDIR=${D} install
}
