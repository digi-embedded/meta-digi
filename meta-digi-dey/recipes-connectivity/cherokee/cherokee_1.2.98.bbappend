# Copyright (C) 2013 Digi International.

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

MIRRORS =+ " \
http://www.cherokee-project.de/mirrors/cherokee/    http://ftp.nluug.nl/internet/cherokee/ \n \
"

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
