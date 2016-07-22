# Copyright (C) 2013 Digi International.

SUMMARY = "DEY examples: OpenGL-ES test application (based on Freescale GPU SDK)"
SECTION = "examples"
LICENSE = "GPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

DEPENDS = "virtual/egl virtual/libgles1 virtual/libgles2"

SRC_URI = "file://opengles"

S = "${WORKDIR}/opengles"

EXTRA_OEMAKE = "EGL_FLAVOUR=${@base_conditional('HAVE_GUI', '1' , 'x11', 'fb', d)}"

do_install () {
	oe_runmake DEST_DIR="${D}" install
}

FILES_${PN} = "/opt/${PN}"
FILES_${PN}-dbg += "/opt/${PN}/.debug"

RDEPENDS_${PN}_ccimx6 = "libopenvg-mx6"

PACKAGE_ARCH = "${MACHINE_ARCH}"

COMPATIBLE_MACHINE = "(ccimx6$)"
