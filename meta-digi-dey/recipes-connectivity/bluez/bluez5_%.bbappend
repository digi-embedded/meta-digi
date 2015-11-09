# Copyright (C) 2015 Digi International.

do_install_append() {
	# gatttool is useful for BLE work but not installed by default
	install -d ${D}${sbindir}
	install -m 0755 attrib/gatttool ${D}${sbindir}/gatttool
}
