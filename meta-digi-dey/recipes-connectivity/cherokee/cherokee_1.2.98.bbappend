# Copyright (C) 2013 Digi International.

PRINC := "${@int(PRINC) + 1}"
PR_append = "+${DISTRO}"

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

SRC_URI = "ftp://ftp.yellowdoglinux.com/.1/cherokee/1.2/${PV}/cherokee-${PV}.tar.gz \
           file://cherokee.init"
SRC_URI[md5sum] = "21b01e7d45c0e82ecc0c4257a9c27feb"
SRC_URI[sha256sum] = "042b5687b1a3db3ca818167548ce5d32c35e227c6640732dcb622a6f4a078b7d"

SRC_URI += " \
	file://cherokee.conf \
	file://digi.gif \
	file://index.html \
	"

do_install_append() {
	install -m 0644 ${WORKDIR}/cherokee.conf ${D}${sysconfdir}/cherokee/
	install -d ${D}/srv/www
	install -m 0644 ${WORKDIR}/index.html ${D}/srv/www/
	install -m 0644 ${WORKDIR}/digi.gif ${D}/srv/www/
}

FILES_${PN} += "/srv/www"
