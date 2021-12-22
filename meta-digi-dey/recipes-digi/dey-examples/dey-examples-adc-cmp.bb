# Copyright (C) 2017-2020 Digi International.

SUMMARY = "DEY examples: Analog Comparator test application"
SECTION = "examples"
LICENSE = "GPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"

SRC_URI = "file://adc_cmp_sample"

S = "${WORKDIR}/adc_cmp_sample"

do_install() {
	install -d ${D}${bindir}
	install -m 0755 adc_cmp_sample ${D}${bindir}
}

PACKAGE_ARCH = "${MACHINE_ARCH}"
COMPATIBLE_MACHINE = "(ccimx6ul|ccimx8x|ccimx8m)"
