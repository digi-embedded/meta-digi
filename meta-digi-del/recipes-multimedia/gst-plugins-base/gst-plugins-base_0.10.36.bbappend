PACKAGECONFIG ??= "${@base_contains('DISTRO_FEATURES', 'x11', 'x11 gio', '', d)}"
PACKAGECONFIG[x11] = ",--disable-x --disable-xshm --disable-xvideo --disable-libvisual,"
PACKAGECONFIG[gio] = ",--disable-gio,"

EXTRA_OECONF += "\
		--disable-cdparanoia \
		--disable-examples \
		--disable-gtk-doc \
		--disable-rpath \
		"
