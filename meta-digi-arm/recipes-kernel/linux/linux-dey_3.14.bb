# Copyright (C) 2015 Digi International

require recipes-kernel/linux/linux-dey.inc
require recipes-kernel/linux/linux-dtb.inc

inherit fsl-vivante-kernel-driver-handler

SRCBRANCH = "v3.14/dey-2.0/maint"
SRCREV = "9aa2120c4aa05526924f8d977a1cec1c3ecdfbee"

COMPATIBLE_MACHINE = "(ccimx6)"
