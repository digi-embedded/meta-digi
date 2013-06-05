# Copyright (C) 2013 Digi International.

PRINC := "${@int(PRINC) + 1}"
PR_append = "+${DISTRO}"

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

# Use 'simple-agent' from bluez-4.98 to avoid a dependence in
# gobject-introspection
SRC_URI += "file://simple-agent"

EXTRA_OECONF_append = " --enable-health"

do_install_append() {
	install -m 0755 ${WORKDIR}/simple-agent ${D}${bindir}
}
