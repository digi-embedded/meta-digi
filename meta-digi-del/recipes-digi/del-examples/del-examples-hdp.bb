SUMMARY = "DEL examples: bluetooth health profile test application"
SECTION = "examples"
LICENSE = "GPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

PR = "r0"

SRC_URI = "file://hdp_test"

S = "${WORKDIR}/hdp_test"

do_install() {
	install -d ${D}${bindir}
	install -m 0755 hdp-test.py ${D}${bindir}
}

RDEPENDS_${PN} = "python"

PACKAGE_ARCH = "${MACHINE_ARCH}"
