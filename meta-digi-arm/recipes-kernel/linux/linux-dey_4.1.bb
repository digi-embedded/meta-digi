# Copyright (C) 2016 Digi International

require recipes-kernel/linux/linux-dey.inc
require recipes-kernel/linux/linux-dtb.inc

SRCBRANCH = "v4.1/dey-2.2/maint"
SRCREV = "${AUTOREV}"

COMPATIBLE_MACHINE = "(ccimx6ul)"
