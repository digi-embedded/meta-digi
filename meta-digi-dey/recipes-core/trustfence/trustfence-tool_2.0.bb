# Copyright (C) 2016 Digi International.

SUMMARY = "Trustfence command line tool"
SECTION = "console/tools"
LICENSE = "CLOSED"

SRC_URI = "file://trustfence-tool"

S = "${WORKDIR}"

INSANE_SKIP_${PN} = "already-stripped"

do_configure[noexec] = "1"
do_compile[noexec] = "1"

do_install() {
	install -d ${D}${base_sbindir}
	install -m 0755 trustfence-tool ${D}${base_sbindir}
}

PACKAGE_ARCH = "${MACHINE_ARCH}"
COMPATIBLE_MACHINE = "(ccimx6|ccimx6ul)"
