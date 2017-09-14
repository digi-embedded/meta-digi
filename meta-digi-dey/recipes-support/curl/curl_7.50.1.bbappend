# Copyright (C) 2017 Digi International Inc.

# 'ares' and 'threaded-resolver' are mutually exclusive
PACKAGECONFIG_append_class-target = " ares"
PACKAGECONFIG[ares] = "--enable-ares,--disable-ares,c-ares"
PACKAGECONFIG[threaded-resolver] = "--enable-threaded-resolver,--disable-threaded-resolver,"
