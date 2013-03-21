# Copyright (C) 2013 Digi International.

DESCRIPTION = "Digi's update test utility"
SECTION = "base"
LICENSE = "GPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

PR = "r0"

DEPENDS += "libdigi"
RDEPENDS_${PN}_append_mxs = " kobs-ng"

SRC_URI = "file://update_flash.c \
           file://jffs2-user.h \
          "

GIT_SHA1 = "$(cd ${THISDIR} && git rev-parse --short HEAD)"

S = "${WORKDIR}"

do_compile() {
	${CC} -O2 -Wall -DGIT_SHA1=\"${GIT_SHA1}\" update_flash.c -o update_flash -ldigi
}

do_install() {
	install -d ${D}${bindir}
	install -m 0755 update_flash ${D}${bindir}
}
