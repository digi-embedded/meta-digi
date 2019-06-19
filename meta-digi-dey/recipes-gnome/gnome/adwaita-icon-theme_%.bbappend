# Copyright (C) 2019 Digi International.

do_install_append() {
	# We don't use the scalable icons anywhere and they take up over
	# 1 MiB in the filesystem, so remove them.
	rm -f ${D}${prefix}/share/icons/Adwaita/scalable/*/*-symbolic*.svg
}

FILES_${PN}-symbolic_remove = "${prefix}/share/icons/Adwaita/scalable/*/*-symbolic*.svg"
