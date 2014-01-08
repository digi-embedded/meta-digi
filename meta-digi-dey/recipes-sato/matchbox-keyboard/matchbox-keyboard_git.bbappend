# Copyright (C) 2014 Digi International.

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

SRC_URI += " \
    file://80matchboxkeyboard.sh \
"

do_install_append () {
	# Remove upstream '80matchboxkeyboard.shbg' file and install ours
	rm -f ${D}/${sysconfdir}/X11/Xsession.d/80matchboxkeyboard.shbg
	install -m 0755 ${WORKDIR}/80matchboxkeyboard.sh ${D}/${sysconfdir}/X11/Xsession.d/
}
