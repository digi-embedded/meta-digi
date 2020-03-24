#
# Copyright (C) 2012-2020 Digi International.
#
SUMMARY = "Bluetooth packagegroup for DEY image"

PACKAGE_ARCH = "${MACHINE_ARCH}"

inherit packagegroup

MACHINE_BLUETOOTH_EXTRA_INSTALL ?= "bluez5-init"

RDEPENDS_${PN} = " \
    bluez5 \
    bluez5-noinst-tools \
    bluez5-obex \
    ${MACHINE_BLUETOOTH_EXTRA_INSTALL} \
"

