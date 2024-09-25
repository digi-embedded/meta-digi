#
# Copyright (C) 2012-2020, Digi International Inc.
#
SUMMARY = "Bluetooth packagegroup for DEY image"

PACKAGE_ARCH = "${MACHINE_ARCH}"

inherit packagegroup

MACHINE_BLUETOOTH_EXTRA_INSTALL ?= "bluez5-init"

RDEPENDS:${PN} = " \
    bluez5 \
    bluez5-noinst-tools \
    bluez5-obex \
    ${MACHINE_BLUETOOTH_EXTRA_INSTALL} \
"

