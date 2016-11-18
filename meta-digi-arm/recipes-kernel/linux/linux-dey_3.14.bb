# Copyright (C) 2015 Digi International

require recipes-kernel/linux/linux-dey.inc
require recipes-kernel/linux/linux-dtb.inc

inherit fsl-vivante-kernel-driver-handler

SRCBRANCH = "v3.14/dey-2.0/maint"
SRCREV = "62c182a10600816e534bfbb960432dd577925e16"

COMPATIBLE_MACHINE = "(ccimx6$)"
