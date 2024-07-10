# Copyright (C) 2013-2022, Digi International Inc.

FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

SRC_URI:append:ccimx6 = " file://0001-sysvinit-disable-all-cpus-but-cpu0-for-halt-reboot.patch"

do_install:append() {
	# Remove 'bootlogd' bootscript symlinks
	update-rc.d -f -r ${D} stop-bootlogd remove
	update-rc.d -f -r ${D} bootlogd remove
}

PACKAGE_ARCH = "${MACHINE_ARCH}"
