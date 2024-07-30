# Copyright (C) 2013-2021, Digi International Inc.

SUMMARY = "Atheros' proprietary tools"
LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/Proprietary;md5=0557f9d92cf58f2ccdd50f62f8ac0b28"

RDEPENDS:${PN} = "libnl libnl-genl libnl-nf libnl-route"

inherit bin_package

SRC_URI = " \
    file://athtestcmd \
"

INSANE_SKIP:${PN} = "already-stripped"

do_install() {
	install -d ${D}${sbindir}
	install -m 0755 ${WORKDIR}/athtestcmd ${D}${sbindir}
}

PACKAGE_ARCH = "${MACHINE_ARCH}"
COMPATIBLE_MACHINE = "(ccimx6sbc)"
