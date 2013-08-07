#
# Copyright (C) 2012 Digi International.
#
SUMMARY = "Wireless packagegroup for DEY image"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/LICENSE;md5=3f40d7994397109285ec7b81fdeb3b58"
PACKAGE_ARCH = "${MACHINE_ARCH}"
ALLOW_EMPTY_${PN} = "1"
PR = "r0"

inherit packagegroup

WIRELESS_MODULE ?= ""
WIRELESS_MODULE_append_mx5 = "${@base_contains('MACHINE_FEATURES', 'wifi', 'kernel-module-redpine', '', d)}"
ATHEROS_WIRELESS_MODULE = '${@base_version_less_or_equal("PREFERRED_VERSION_linux-dey", "2.6.35.14", "kernel-module-atheros", "", d)}'
WIRELESS_MODULE_append_mxs = "${@base_contains('MACHINE_FEATURES', 'wifi', '${ATHEROS_WIRELESS_MODULE}', '', d)}"

RDEPENDS_${PN} = "\
	wpa-supplicant \
	wireless-tools \
	crda \
	${WIRELESS_MODULE} \
"

RDEPENDS_${PN}_append_mx5 = "${WIRELESS_MODULE}"
RDEPENDS_${PN}_append_mxs = " iw ${WIRELESS_MODULE}"
