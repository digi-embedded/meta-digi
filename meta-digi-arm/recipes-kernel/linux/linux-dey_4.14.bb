# Copyright (C) 2019,2020 Digi International

require recipes-kernel/linux/linux-dey.inc

SRC_URI += "file://0001-compiler-attributes-add-support-for-copy-gcc-9.patch \
            file://0002-include-linux-module.h-copy-init-exit-attrs-to-.patch \
           "

COMPATIBLE_MACHINE = "(ccimx6ul|ccimx8x|ccimx8m)"
