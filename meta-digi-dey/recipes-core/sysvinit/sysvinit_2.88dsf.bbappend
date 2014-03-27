# Copyright (C) 2013 Digi International.

# Remove 'bootlogd' bootscript and symlinks
# (synchronize with poky's sysvinit_2.88dsf.bb)
do_install_append() {
	rm -f ${D}${sysconfdir}/init.d/stop-bootlogd
	rm -f ${D}${sysconfdir}/init.d/bootlogd
	rm -f ${D}${sysconfdir}/rc2.d/S99stop-bootlogd
	rm -f ${D}${sysconfdir}/rc3.d/S99stop-bootlogd
	rm -f ${D}${sysconfdir}/rc4.d/S99stop-bootlogd
	rm -f ${D}${sysconfdir}/rc5.d/S99stop-bootlogd
	rm -f ${D}${sysconfdir}/rcS.d/S07bootlogd
}

do_install_append_ccimx6adpt() {
	cat >> ${D}${sysconfdir}/default/rcS <<-EOF
		# Resize EXT4 filesystems to the size of the partition on boot
		RESIZE_EXT4FS=yes
	EOF
}
