# Copyright (C) 2011, 2012 Freescale
# Copyright (C) 2012 Digi International
# Released under the MIT license (see COPYING.MIT for the terms)

DESCRIPTION = "AMD libz160 gpu driver"
LICENSE = "Proprietary"
SECTION = "libs"
PR = "r2"

#todo: Replace for correct AMD license
LIC_FILES_CHKSUM = "file://usr/include/z160.h;endline=28;md5=65dd44cd769091092f38e34cd52cc271"

SRC_URI = "${DIGI_LOG_MIRROR}/libz160-bin-${PV}.tar.gz"
SRC_URI[md5sum] = "49b6d51e2ea6651107b08f43715c8c2e"
SRC_URI[sha256sum] = "43b1bebb2656d0c868c10f66ddc064c6324b74694daedfb3f542f93f438232c5"

do_install () {
    install -d ${D}${libdir}
    install -d ${D}${includedir}
    install -m 0755 ${S}/usr/lib/* ${D}${libdir}
    install -m 0644 ${S}/usr/include/* ${D}${includedir}
}

S = "${WORKDIR}/${PN}-bin-${PV}"

# Avoid QA Issue: No GNU_HASH in the elf binary
INSANE_SKIP_${PN} = "ldflags"
INSANE_SKIP_${PN}-dev = "ldflags"
FILES_${PN} = "${libdir}/*.so"
FILES_${PN}-dev = "${includedir}"
