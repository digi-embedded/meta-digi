# Copyright (C) 2019-2025, Digi International Inc.

do_install:append() {
	# We don't use the scalable icons anywhere and they take up over
	# 1 MiB in the filesystem, so remove them.
	rm -f ${D}${prefix}/share/icons/Adwaita/scalable/*/*-symbolic*.svg
}

FILES:${PN}-symbolic:remove = "${prefix}/share/icons/Adwaita/scalable/*/*-symbolic*.svg"

# librsvg-native was needed in previous versions of adwaita-icon-theme, which
# used gtk-encode-symbolic-svg to create PNG versions of SVG files. This is no
# longer the case and the dependency can be removed without consequence.
# In turn, this removes the only dependency with Rust in ccimx6ul/x11 images,
# reducing build time overhead.
DEPENDS:remove = "librsvg-native"
