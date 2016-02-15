# Copyright (C) 2015 Digi International

require recipes-kernel/linux/linux-dey.inc
require recipes-kernel/linux/linux-dtb.inc

inherit fsl-vivante-kernel-driver-handler

SRCBRANCH = "v3.14/dey-2.0/maint"
SRCREV = "c08e6c87f47fefff1fb2dcf37012d1a442fa872f"

COMPATIBLE_MACHINE = "(ccimx6)"
