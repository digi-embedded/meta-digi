# Copyright (C) 2019 Digi International.

do_install:append() {
	# We don't use the scalable icons anywhere and they take up over
	# 1 MiB in the filesystem, so remove them.
	rm -f ${D}${prefix}/share/icons/Adwaita/scalable/*/*-symbolic*.svg
}

FILES:${PN}-symbolic:remove = "${prefix}/share/icons/Adwaita/scalable/*/*-symbolic*.svg"
