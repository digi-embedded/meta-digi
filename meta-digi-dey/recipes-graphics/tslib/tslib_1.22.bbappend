# Copyright (C) 2022, Digi International Inc.

# Crank storyboard engine needs 'libts-1.0.so.0'
do_install:append() {
	ln -s libts.so.0 ${D}${libdir}/libts-1.0.so.0
}

RPROVIDES:${PN} = "libts-1.0.so.0"
