# Copyright (C) 2013-2014 Digi International.

do_install_append() {
	# Remove 'bootlogd' bootscript symlinks
	update-rc.d -f -r ${D} stop-bootlogd remove
	update-rc.d -f -r ${D} bootlogd remove
}

do_install_append_ccimx6() {
	cat >> ${D}${sysconfdir}/default/rcS <<-EOF
		# Resize EXT4 filesystems to the size of the partition on boot
		RESIZE_EXT4FS=yes
	EOF
}
