# Copyright 2017, Digi International Inc.

FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

SRC_URI += "file://cellularifupdown"

# 'polkit' depends on 'consolekit', and this requires 'x11' distro feature. So
# remove 'polkit' support to be able to build ModemManager on a framebuffer
# only image (without X11)
PACKAGECONFIG_remove = " polkit"

do_install_append() {
	# Install ifupdown script for cellular interfaces
	install -d ${D}${sysconfdir}/network/if-pre-up.d/ ${D}${sysconfdir}/network/if-post-down.d/
	install -m 0755 ${WORKDIR}/cellularifupdown ${D}${sysconfdir}/network/if-pre-up.d/
	ln -sf ../if-pre-up.d/cellularifupdown ${D}${sysconfdir}/network/if-post-down.d/cellularifupdown
}
