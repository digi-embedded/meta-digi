#
# Copyright (C) 2012 Digi International.
#
SUMMARY = "Bluetooth packagegroup for DEY image"

PACKAGE_ARCH = "${MACHINE_ARCH}"
inherit packagegroup bluetooth

RDEPENDS_${PN} = " \
	${BLUEZ} \
	${BLUEZ}-testtools \
	${@bb.utils.contains("MACHINE_FEATURES", "bluetooth", "libasound-module-bluez", "", d)} \
	${@bb.utils.contains("BLUEZ", "bluez5", "bluez5-noinst-tools bluez5-obex", "", d)} \
"
