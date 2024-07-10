# Copyright (C) 2022, Digi International Inc.

FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

SRC_URI:append = " file://0001-wolfcrypttest-fix-for-FIPS-enabled-wolfSSL-library.patch"

CFLAGS:append:arm = " -DSIZEOF_LONG=4 -DSIZEOF_LONG_LONG=8"
CFLAGS:append:aarch64 = " -DSIZEOF_LONG=8 -DSIZEOF_LONG_LONG=8"
