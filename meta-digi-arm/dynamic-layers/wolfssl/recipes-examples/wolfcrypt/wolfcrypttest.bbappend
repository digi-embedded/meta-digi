# Copyright (C) 2022 Digi International

CFLAGS:append:arm = " -DSIZEOF_LONG=4 -DSIZEOF_LONG_LONG=8"
CFLAGS:append:aarch64 = " -DSIZEOF_LONG=8 -DSIZEOF_LONG_LONG=8"
