#
# Copyright (C) 2012-2018 Digi International.
#
SUMMARY = "Bluetooth packagegroup for DEY image"

PACKAGE_ARCH = "${MACHINE_ARCH}"
inherit packagegroup bluetooth

MACHINE_BLUETOOTH_EXTRA_INSTALL ?= "${@bb.utils.contains('BLUEZ', 'bluez5', 'bluez5-init', '', d)}"

RDEPENDS_${PN} = " \
    ${BLUEZ} \
    ${@bb.utils.contains('BLUEZ', 'bluez5', 'bluez5-noinst-tools bluez5-obex', '', d)} \
    ${MACHINE_BLUETOOTH_EXTRA_INSTALL} \
"

