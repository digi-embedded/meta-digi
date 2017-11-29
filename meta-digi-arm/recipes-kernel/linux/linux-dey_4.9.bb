# Copyright (C) 2017 Digi International

require recipes-kernel/linux/linux-dey.inc
require recipes-kernel/linux/linux-dtb.inc

SRCBRANCH = "v4.9/dey-2.2/maint"
SRCREV = "${AUTOREV}"

COMPATIBLE_MACHINE = "(ccimx6qpsbc|ccimx6ul)"
