# Copyright (C) 2013 Digi International.

SUMMARY = "DEY examples: SPI device driver test application"
SECTION = "examples"
LICENSE = "GPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

SRC_URI = "file://spidev_test"

S = "${WORKDIR}/spidev_test"

python do_warning_spidev() {
    pass
}

# Warn the user in case we cannot enable spidev in the device tree
python do_warning_spidev_ccardimx28() {
    if d.getVar('HAVE_GUI', True) and not d.getVar('IS_KERNEL_2X', True):
        bb.warn("SPIDEV conflicts with touchscreen so it was not enabled in the device tree")
}
addtask warning_spidev before do_compile

do_compile() {
	${CC} -O2 -Wall spidev_test.c -o spidev_test
}

do_install() {
	install -d ${D}${bindir}
	install -m 0755 spidev_test ${D}${bindir}
}

PACKAGE_ARCH = "${MACHINE_ARCH}"
