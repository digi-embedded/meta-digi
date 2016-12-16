# Copyright (C) 2016 Digi International

FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

# Version 2016.10
SRCREV = "edd6559728d2e234ebdc03ba1d5444449ae2b92b"
PV = "2016.10+git${SRCPV}"

do_install_append() {
	# The 'progress' command is new starting in version '2016.10', but we
	# don't need to do a version check here because the bbappend is version
	# specific (PV hardcoded above)
	install -d ${D}${bindir}/
	install -m 0755 progress ${D}${bindir}/
}
