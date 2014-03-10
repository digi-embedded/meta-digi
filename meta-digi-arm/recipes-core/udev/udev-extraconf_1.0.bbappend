# Copyright (C) 2013 Digi International.

PRINC := "${@int(PRINC) + 1}"
PR_append = "+${DISTRO}"

FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI += "file://mount_bootparts.sh"

do_install_append() {
	install -m 0755 ${WORKDIR}/mount_bootparts.sh ${D}${sysconfdir}/udev/scripts/
}
