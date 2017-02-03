# Copyright (C) 2015 Digi International

require recipes-kernel/linux/linux-dey.inc
require recipes-kernel/linux/linux-dtb.inc

inherit fsl-vivante-kernel-driver-handler

SRCBRANCH = "v3.14/dey-2.0/maint"
SRCREV = "c4cef439604bdfa65d5aeb0b90ef429506a8ec9d"

COMPATIBLE_MACHINE = "(ccimx6$)"
