#
# Copyright (C) 2012-2021 Digi International.
#
SUMMARY = "Wireless packagegroup for DEY image"

PACKAGE_ARCH = "${MACHINE_ARCH}"
inherit packagegroup

RDEPENDS_${PN} = "\
    hostapd \
    iw \
    wireless-regdb-static \
    wpa-supplicant \
    wpa-supplicant-cli \
    wpa-supplicant-passphrase \
"

RDEPENDS_${PN}_append_ccimx6sbc = " ath-prop-tools"
RDEPENDS_${PN}_remove_ccimx6sbc = "hostapd"
