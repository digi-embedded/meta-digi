#
# Copyright (C) 2012-2026, Digi International Inc.
#
SUMMARY = "Wireless packagegroup for DEY image"

PACKAGE_ARCH = "${MACHINE_ARCH}"
inherit packagegroup

RDEPENDS:${PN} = "\
    hostapd \
    iw \
    wireless-regdb-static \
    wpa-supplicant \
    wpa-supplicant-cli \
    wpa-supplicant-passphrase \
"

RDEPENDS:${PN}:append:ccimx6sbc = " ath-prop-tools"
RDEPENDS:${PN}:append:ccimx91 = " nxp-wlan-utils"
RDEPENDS:${PN}:append:ccimx93 = " nxp-wlan-utils"
