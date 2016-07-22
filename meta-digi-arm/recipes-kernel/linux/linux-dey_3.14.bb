# Copyright (C) 2015 Digi International

require recipes-kernel/linux/linux-dey.inc
require recipes-kernel/linux/linux-dtb.inc

inherit fsl-vivante-kernel-driver-handler

SRCBRANCH = "v3.14/dey-2.0/maint"
SRCREV = "1d2fe70cd126a1102ae1db0e8b214ac5f064521f"

COMPATIBLE_MACHINE = "(ccimx6$)"
