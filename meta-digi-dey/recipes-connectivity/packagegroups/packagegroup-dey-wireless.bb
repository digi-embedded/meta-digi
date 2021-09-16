#
# Copyright (C) 2012-2020 Digi International.
#
SUMMARY = "Wireless packagegroup for DEY image"

PACKAGE_ARCH = "${MACHINE_ARCH}"
inherit packagegroup

RDEPENDS_${PN} = "\
    crda \
    hostapd \
    iw \
    wpa-supplicant \
    wpa-supplicant-cli \
    wpa-supplicant-passphrase \
"
