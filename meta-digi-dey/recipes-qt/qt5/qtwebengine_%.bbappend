# Copyright (C) 2015 Digi International

# Decrease memory used by the linker to avoid being killed due to
# out of memory.
LDFLAGS += "-Wl,--no-keep-memory"

# To avoid the OOM killer, decrease parallel make jobs for this specific recipe.
python __anonymous () {
    makejobs = int(d.getVar('PARALLEL_MAKE', True).split()[1]) // 2
    d.setVar("PARALLEL_MAKE", "-j %d" % (makejobs, 1)[makejobs == 0])
}
export NINJAFLAGS = "${PARALLEL_MAKE}"
