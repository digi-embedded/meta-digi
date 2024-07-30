# Copyright (C) 2017, Digi International Inc.

SUMMARY = "DEY sound card detection app"
SECTION = "multimedia"
LICENSE = "MPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MPL-2.0;md5=815ca599c9df247a0c7f619bab123dad"

DEPENDS = "alsa-lib"

SRC_URI = "file://card-detect.c"

S = "${WORKDIR}"

inherit pkgconfig

export CFLAGS += "`pkg-config --cflags alsa`"
export LDLIBS += "`pkg-config --libs alsa`"

do_configure[noexec] = "1"

do_compile() {
	oe_runmake card-detect
}

do_install() {
	install -d ${D}${bindir}
	install -m 0755 card-detect ${D}${bindir}
}
