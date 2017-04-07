# Copyright (C) 2016, 2017 Digi International

require recipes-kernel/linux/linux-dey.inc
require recipes-kernel/linux/linux-dtb.inc

SRCBRANCH = "v4.1.15/master"
SRCREV = "${AUTOREV}"

COMPATIBLE_MACHINE = "(ccimx6$|ccimx6ul)"
