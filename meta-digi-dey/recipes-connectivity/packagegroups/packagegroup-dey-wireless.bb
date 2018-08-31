#
# Copyright (C) 2012-2018 Digi International.
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
RDEPENDS_${PN}_append_ccimx6qpsbc = " hostapd"
RDEPENDS_${PN}_append_ccimx8x = " hostapd"
