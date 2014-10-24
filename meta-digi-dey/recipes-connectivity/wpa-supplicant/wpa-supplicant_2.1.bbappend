# Copyright (C) 2013 Digi International.

FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI += " \
    file://defconfig \
    file://0001-events-Reduce-verbosity-of-scan-events.patch \
"

do_configure () {
	install -m 0755 ${WORKDIR}/defconfig wpa_supplicant/.config
	echo "CFLAGS +=\"-I${STAGING_INCDIR}/libnl3\"" >> wpa_supplicant/.config
}
