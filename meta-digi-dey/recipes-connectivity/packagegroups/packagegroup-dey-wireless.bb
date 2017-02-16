#
# Copyright (C) 2012 Digi International.
#
SUMMARY = "Wireless packagegroup for DEY image"

PACKAGE_ARCH = "${MACHINE_ARCH}"
inherit packagegroup

RDEPENDS_${PN} = "\
    crda \
    iw \
    wireless-tools \
    wpa-supplicant \
    wpa-supplicant-cli \
    wpa-supplicant-passphrase \
"

RDEPENDS_${PN}_append_ccimx6ul = " hostapd"
