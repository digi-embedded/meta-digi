# Copyright (C) 2019 Digi International

require recipes-kernel/linux/linux-dey.inc

SRCBRANCH = "v4.20.y"
SRCREV = "${AUTOREV}"

DEPENDS += "openssl-native"
HOST_EXTRACFLAGS += "-I${STAGING_INCDIR_NATIVE}"

COMPATIBLE_MACHINE = "(ccimx6ul)"
