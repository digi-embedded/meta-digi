# Copyright (C) 2025, Digi International Inc.

DESCRIPTION = "Prebuilt Wi-Fi tools for NXP IW61x chip"
SECTION = "Binaries"
LICENSE = "CLOSED"
SRC_URI = " \
	file://mlanutl \
	file://mlanwls \
	file://nanapp \
"

S = "${WORKDIR}"

INSANE_SKIP:${PN} += "already-stripped file-rdeps"

do_install() {
    install -d ${D}${bindir}
    install -m 0755 ${WORKDIR}/mlanutl ${D}${bindir}
    install -m 0755 ${WORKDIR}/mlanwls ${D}${bindir}
    install -m 0755 ${WORKDIR}/nanapp ${D}${bindir}
}

COMPATIBLE_MACHINE = "(ccimx9)"
