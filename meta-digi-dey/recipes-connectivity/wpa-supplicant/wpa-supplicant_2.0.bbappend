# Copyright (C) 2013 Digi International.

PRINC := "${@int(PRINC) + 1}"
PR_append = "+${DISTRO}"

FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

# We use OPENSSL for TLS implementation
DEPENDS := "${@oe_filter_out('gnutls', DEPENDS, d)}"
DEPENDS += "openssl"

SRC_URI += "file://defconfig"

do_configure () {
	install -m 0755 ${WORKDIR}/defconfig wpa_supplicant/.config
	echo "CFLAGS +=\"-I${STAGING_INCDIR}/libnl3\"" >> wpa_supplicant/.config
}
