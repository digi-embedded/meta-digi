# Copyright (C) 2016 Digi International.

FILESEXTRAPATHS_prepend := "${THISDIR}/${BP}:"

# The recipe uses a different "$S" directory so point the patch to the hostapd
# tarball directory.
SRC_URI_append_ccimx6ul = " file://fix_num_probereq_cb_clearing.patch;patchdir=.."
SRC_URI += "file://hostapd.conf"

do_install_append() {
	# Overwrite the default hostapd.conf with our custom file
	install -m 0644 ${WORKDIR}/hostapd.conf ${D}${sysconfdir}/hostapd.conf
}

# Do not autostart hostapd daemon, it will conflict with wpa-supplicant.
INITSCRIPT_PARAMS = "remove"

PACKAGE_ARCH = "${MACHINE_ARCH}"
