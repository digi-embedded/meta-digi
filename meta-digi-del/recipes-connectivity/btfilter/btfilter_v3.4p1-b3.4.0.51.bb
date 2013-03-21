DESCRIPTION = "Atheros BT/wlan coexistance daemon"
SECTION = "network"
LICENSE = "ISC"
LIC_FILES_CHKSUM = "file://Makefile;beginline=1;endline=14;md5=8f6614b37751445a5f6a9bdc69be26b3"

DEPENDS = "bluez4 dbus libnl"

PR = "r0"

SRC_URI = "${DIGI_MIRROR}/${PN}-${PV}.tar.bz2 \
	   file://0001-enable-libnl3.patch \
	   file://0002-cross-compile.patch \
	   file://bluez-init"

SRC_URI[md5sum] = "149b025f7a43f1f3abfa12462c48559a"
SRC_URI[sha256sum] = "bbb358ce25ec36b32f99e66036ff52f375f3c1272b1425fafbef2c240a55d1a4"

inherit update-rc.d

EXTRA_OEMAKE = "INCLUDES=-I${STAGING_INCDIR}/bluetooth"

do_install() {
	install -d ${D}${bindir} ${D}${sysconfdir}/init.d/
	install -m 0755 abtfilt ${D}${bindir}
	install -m 0755 ${WORKDIR}/bluez-init ${D}${sysconfdir}/init.d/bluez
}

INITSCRIPT_NAME = "bluez"
INITSCRIPT_PARAMS = "start 10 5 ."
