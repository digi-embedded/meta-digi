# Copyright (C) 2013-2018 Digi International.

FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

SRC_URI += " \
    file://cherokee.conf \
    file://cherokee.pem \
    file://digi-logo.png \
    file://index.html \
"

do_install:append() {
	install -d ${D}${sysconfdir}/cherokee/ssl ${D}/srv/www
	install -m 0644 ${WORKDIR}/cherokee.conf ${D}${sysconfdir}/cherokee/
	install -m 0644 ${WORKDIR}/cherokee.pem ${D}${sysconfdir}/cherokee/ssl/
	install -m 0644 ${WORKDIR}/index.html ${D}/srv/www/
	install -m 0644 ${WORKDIR}/digi-logo.png ${D}/srv/www/
}

FILES:${PN} += "/srv/www"
