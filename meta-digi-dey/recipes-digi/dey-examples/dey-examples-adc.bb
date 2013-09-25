# Copyright (C) 2013 Digi International.

SUMMARY = "DEY examples: ADC test application"
SECTION = "examples"
LICENSE = "GPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

DEPENDS = "virtual/kernel"

PR = "${DISTRO}.r0"

SRC_URI = "file://adc_test"

UPPER_PLAT = "${@'${MACHINE}'.upper()}"

S = "${WORKDIR}/adc_test"

do_compile() {
	${CC} -O2 -Wall -I${STAGING_KERNEL_DIR}/include -D${UPPER_PLAT} adc_test.c -o adc_test -lm
}

do_install() {
	install -d ${D}${bindir}
	install -m 0755 adc_test ${D}${bindir}
}

PACKAGE_ARCH = "${MACHINE_ARCH}"

COMPATIBLE_MACHINE = "(ccardimx28js|ccimx51js|ccimx53js)"
