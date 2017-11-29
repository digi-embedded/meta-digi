# Copyright (C) 2015-2017 Digi International

require recipes-kernel/linux/linux-dey.inc
require recipes-kernel/linux/linux-dtb.inc

inherit fsl-vivante-kernel-driver-handler

SRCBRANCH = "v3.14/dey-2.0/maint"
SRCREV = "${AUTOREV}"

COMPATIBLE_MACHINE = "(ccimx6sbc)"
