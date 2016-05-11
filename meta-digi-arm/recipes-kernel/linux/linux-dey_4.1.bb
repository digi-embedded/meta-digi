# Copyright (C) 2016 Digi International

require recipes-kernel/linux/linux-dey.inc
require recipes-kernel/linux/linux-dtb.inc

SRCBRANCH_ccimx6ul = "v4.1/master"
SRCREV = "${AUTOREV}"

COMPATIBLE_MACHINE = "(ccimx6ul)"
