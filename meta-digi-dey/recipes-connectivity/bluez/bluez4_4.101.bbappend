# Copyright (C) 2013 Digi International.

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

# Use 'simple-agent' from bluez-4.98 to avoid a dependence in
# gobject-introspection
SRC_URI += "file://simple-agent"

# 'simple-agent' needs some python packages
RDEPENDS_${PN} = "python-dbus python-pygobject"

EXTRA_OECONF_append = " --enable-health"

do_install_append() {
	install -m 0755 ${WORKDIR}/simple-agent ${D}${bindir}
}
