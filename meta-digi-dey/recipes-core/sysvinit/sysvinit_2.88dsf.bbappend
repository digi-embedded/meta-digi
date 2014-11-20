# Copyright (C) 2013-2014 Digi International.

do_install_append() {
	# Remove 'bootlogd' bootscript symlinks
	update-rc.d -f -r ${D} stop-bootlogd remove
	update-rc.d -f -r ${D} bootlogd remove
}
