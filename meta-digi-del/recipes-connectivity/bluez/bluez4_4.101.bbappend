FILESEXTRAPATHS_prepend := "${THISDIR}/files:"
PR_append = "+${DISTRO}.r0"

# Use 'simple-agent' from bluez-4.98 to avoid a dependence in
# gobject-introspection
SRC_URI += "file://simple-agent"

EXTRA_OECONF_append_del = " --enable-health"

do_install_append() {
	install -m 0755 ${WORKDIR}/simple-agent ${D}${bindir}
}
