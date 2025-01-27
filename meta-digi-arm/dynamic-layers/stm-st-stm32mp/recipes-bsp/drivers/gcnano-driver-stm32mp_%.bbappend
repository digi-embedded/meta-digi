# Copyright (C) 2025, Digi International Inc.

FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

SRC_URI:append = " \
    file://0001-gc_hal_kernel_command-fix-Kernel-NULL-pointer-on-gck.patch\
"
