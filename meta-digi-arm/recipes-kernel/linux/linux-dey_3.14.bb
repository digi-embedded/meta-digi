# Copyright (C) 2015 Digi International

require recipes-kernel/linux/linux-dey.inc
require recipes-kernel/linux/linux-dtb.inc

inherit fsl-vivante-kernel-driver-handler

SRCBRANCH = "v3.14/dey-2.0/maint"
SRCREV = "0b02f8ecf5ee674751f929429f9a3ac8b51138f4"

COMPATIBLE_MACHINE = "(ccimx6)"
