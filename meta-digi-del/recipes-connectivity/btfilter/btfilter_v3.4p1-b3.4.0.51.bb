SUMMARY = "Atheros BT/wlan coexistance daemon"
SECTION = "network"
LICENSE = "ISC"
LIC_FILES_CHKSUM = "file://Makefile;beginline=1;endline=14;md5=8f6614b37751445a5f6a9bdc69be26b3"

DEPENDS = "libnl bluez4"

PR = "r0"

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"
SRC_URI = "${DIGI_LOG_MIRROR}${PN}-${PV}.tar.bz2 \
           file://0001-enable-libnl3.patch \
	   file://S65bluez-bg.sh"

S = "${WORKDIR}/${PN}-${PV}"

EXTRA_OEMAKE += "V210_DIR=${STAGING_DIR_TARGET}"
CFLAGS_prepend = "-I ${STAGING_DIR_TARGET}/usr/include -DCONFIG_LIBNL20 -DCONFIG_NO_HCILIBS"

SRC_URI[md5sum] = "149b025f7a43f1f3abfa12462c48559a"
SRC_URI[sha256sum] = "bbb358ce25ec36b32f99e66036ff52f375f3c1272b1425fafbef2c240a55d1a4"

do_install() {
        install -d ${D}/${bindir}
        install -m 0755 ${S}/abtfilt ${D}${bindir}/abtfilt
	install -d ${D}${sysconfdir}/rc5.d
        install -m 0755 ${WORKDIR}/S65bluez-bg.sh ${D}${sysconfdir}/rc5.d/
}

FILES_${PN} = "${bindir}/abtfilt"
FILES_${PN} += "${sysconfdir}/rc5.d/S65bluez-bg.sh"
CONFFILES_${PN} += "${sysconfdir}/rc5.d/S65bluez-bg.sh"
