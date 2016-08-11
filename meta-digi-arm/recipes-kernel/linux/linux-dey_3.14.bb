# Copyright (C) 2015 Digi International

require recipes-kernel/linux/linux-dey.inc
require recipes-kernel/linux/linux-dtb.inc

inherit fsl-vivante-kernel-driver-handler

SRCBRANCH = "v3.14/dey-2.0/maint"
SRCREV = "5542184056ec555f314b4b8fce8f3b0fbb87168a"

COMPATIBLE_MACHINE = "(ccimx6$)"
