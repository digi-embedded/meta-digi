SUMMARY = "DEL examples: application to perform low level bluetooth"
SECTION = "examples"
LICENSE = "GPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

DEPENDS = "bluez4"

PR = "r0"

SRC_URI = "file://btconfig"

S = "${WORKDIR}/btconfig"

do_compile() {
	${CC} -O2 -Wall btconfig.c -o btconfig -lbluetooth
}

do_install() {
	install -d ${D}${bindir}
	install -m 0755 btconfig ${D}${bindir}
}

PACKAGE_ARCH = "${MACHINE_ARCH}"
