# Copyright (C) 2015 Digi International

# Decrease memory used by the linker to avoid being killed due to
# out of memory.
LDFLAGS += "-Wl,--no-keep-memory"
