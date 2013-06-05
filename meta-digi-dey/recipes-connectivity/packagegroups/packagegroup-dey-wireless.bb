#
# Copyright (C) 2012 Digi International.
#
SUMMARY = "Wireless packagegroup for DEY image"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/LICENSE;md5=3f40d7994397109285ec7b81fdeb3b58"
PACKAGE_ARCH = "${MACHINE_ARCH}"
ALLOW_EMPTY = "1"
PR = "r0"

inherit packagegroup

#
# Set by the machine configuration with packages essential for device bootup
#
MACHINE_ESSENTIAL_EXTRA_RDEPENDS ?= ""
MACHINE_ESSENTIAL_EXTRA_RRECOMMENDS ?= ""

WIRELESS_MODULE ?= ""
WIRELESS_MODULE_append_mx5 = "${@base_contains('MACHINE_FEATURES', 'wifi', 'kernel-module-redpine', '', d)}"
WIRELESS_MODULE_append_mxs = "${@base_contains('MACHINE_FEATURES', 'wifi', 'kernel-module-atheros', '', d)}"

RDEPENDS_${PN} = "\
	wpa-supplicant \
	wireless-tools \
	crda \
	${WIRELESS_MODULE} \
	${MACHINE_ESSENTIAL_EXTRA_RDEPENDS}"

RDEPENDS_${PN}_append_ccimx51js = "kernel-module-redpine"
RDEPENDS_${PN}_append_ccimx53js = "kernel-module-redpine"
RDEPENDS_${PN}_append_ccardimx28js = " iw kernel-module-atheros"
RDEPENDS_${PN}_append_cpx2 = " iw kernel-module-atheros"

RRECOMMENDS_${PN} = "\
    ${MACHINE_ESSENTIAL_EXTRA_RRECOMMENDS}"
