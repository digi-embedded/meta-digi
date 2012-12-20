SUMMARY = "Atheros BT/wlan coexistance daemon"
SECTION = "network"
LICENSE = "ISC"
LIC_FILES_CHKSUM = "file://Makefile;beginline=1;endline=14;md5=8f6614b37751445a5f6a9bdc69be26b3"

DEPENDS = "libnl bluez4"

PR = "r0"

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"
SRC_URI = "${DIGI_LOG_MIRROR}${PN}-${PV}.tar.bz2 \
	   file://S65bluez-bg.sh"

S = "${WORKDIR}/${PN}-${PV}"

EXTRA_OEMAKE += "V210_DIR=${STAGING_DIR_TARGET}"
CFLAGS_prepend = "-I ${STAGING_DIR_TARGET}/usr/include -DCONFIG_LIBNL20 -DCONFIG_NO_HCILIBS"

SRC_URI[md5sum] = "5218950e9351a05ff98246de5b0a9139"
SRC_URI[sha256sum] = "2fa79c8c29f11ecae9303e93cbba8df6ee344df987672794e2939b72c171f5e8"

do_install() {
        install -d ${D}/${bindir}
        install -m 0755 ${S}/abtfilt ${D}${bindir}/abtfilt
	install -d ${D}${sysconfdir}/rcS.d
        install -m 0755 ${WORKDIR}/S65bluez-bg.sh ${D}${sysconfdir}/rcS.d/
}

FILES_${PN} = "${bindir}/abtfilt"
FILES_${PN} += "${sysconfdir}/rcS.d/S65bluez-bg.sh"
CONFFILES_${PN} += "${sysconfdir}/rcS.d/S65bluez-bg.sh"
