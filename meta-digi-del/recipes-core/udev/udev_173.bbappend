# Copyright (C) 2013 Digi International.

PRINC := "${@int(PRINC) + 1}"
PR_append = "+${DISTRO}"

FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}-${PV}:"

RRECOMMENDS_${PN} += "udev-extraconf"

do_install_append() {
	# Remove empty dir to clean build warning:
	# QA Issue: udev: Files/directories were installed but not shipped
	rmdir ${D}${sbindir}
}
