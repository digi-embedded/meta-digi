# Copyright (C) 2015 Digi International

require recipes-kernel/linux/linux-dey.inc
require recipes-kernel/linux/linux-dtb.inc

inherit fsl-vivante-kernel-driver-handler

SRCBRANCH = "v3.14/dey-2.0/maint"
SRCREV = "973260ccfe67e7f47c549fadba27f77adb8f7982"

COMPATIBLE_MACHINE = "(ccimx6$)"
