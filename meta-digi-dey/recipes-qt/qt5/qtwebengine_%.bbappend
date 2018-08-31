# Copyright (C) 2015-2018 Digi International

do_install_append() {
	if ls ${D}${libdir}/pkgconfig/Qt5*.pc >/dev/null 2>&1; then
		sed -i 's,-L${STAGING_DIR_HOST}/usr/lib,,' ${D}${libdir}/pkgconfig/Qt5*.pc
	fi
}

COMPATIBLE_MACHINE_aarch64 = "(.*)"

# Decrease memory used by the linker to avoid being killed due to
# out of memory.
LDFLAGS += "-Wl,--no-keep-memory"

# To avoid the OOM killer, decrease parallel make jobs for this specific recipe.
python __anonymous () {
    makejobs = int(d.getVar('PARALLEL_MAKE', True).split()[1]) // 2
    d.setVar("PARALLEL_MAKE", "-j %d" % (makejobs, 1)[makejobs == 0])
}
export NINJAFLAGS = "${PARALLEL_MAKE}"
