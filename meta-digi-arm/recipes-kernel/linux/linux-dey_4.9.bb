# Copyright (C) 2017 Digi International

require recipes-kernel/linux/linux-dey.inc
require recipes-kernel/linux/linux-dtb.inc

SRCBRANCH = "v4.9.11/master"
SRCREV = "${AUTOREV}"

COMPATIBLE_MACHINE = "(ccimx6ul)"
