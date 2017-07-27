# Copyright (C) 2015 Digi International

require recipes-kernel/linux/linux-dey.inc
require recipes-kernel/linux/linux-dtb.inc

inherit fsl-vivante-kernel-driver-handler

SRCBRANCH = "v3.14/dey-2.0/maint"
SRCREV = "eaa1199a097217aa619817654e022f2605c45be6"

COMPATIBLE_MACHINE = "(ccimx6$)"
