# Copyright (C) 2017 Digi International.

SUMMARY = "DEY sound card detection app"
SECTION = "multimedia"
LICENSE = "MPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MPL-2.0;md5=815ca599c9df247a0c7f619bab123dad"

DEPENDS = "alsa-lib"

SRC_URI = "file://card-detect.c"

S = "${WORKDIR}"

do_compile() {
	${CC} -O2 -Wall card-detect.c -o card-detect -lasound
}

do_install() {
	install -d ${D}${bindir}
	install -m 0755 card-detect ${D}${bindir}
}
