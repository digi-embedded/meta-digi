# Copyright (C) 2013 Digi International.

PRINC := "${@int(PRINC) + 1}"
PR_append = "+${DISTRO}"

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

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
