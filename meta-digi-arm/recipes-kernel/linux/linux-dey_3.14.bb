# Copyright (C) 2015 Digi International

require recipes-kernel/linux/linux-dey.inc
require recipes-kernel/linux/linux-dtb.inc

inherit fsl-vivante-kernel-driver-handler

SRCBRANCH = "v3.14/dey-2.0/maint"
SRCREV = "18747e099eccc726c3abc44b7fbe537ffcce186f"

COMPATIBLE_MACHINE = "(ccimx6$)"
