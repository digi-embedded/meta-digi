# Copyright (C) 2013 Digi International.

# The initscripts package includes a '/etc/init.d/umountfs' script used for
# reboot/poweroff purposes. That script calls the 'umount' command but seems
# that the combination of busybox' umount + linux 3.x fails in this script (it
# hangs without completing the reboot/poweroff)
# So as a workaround use the umount command from util-linux package
RDEPENDS_${PN} = "util-linux-umount"

FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}-${PV}:"

SRC_URI += "file://device_table.txt"

do_install_append() {
	install -m 0755 ${WORKDIR}/device_table.txt ${D}${sysconfdir}/device_table
}
