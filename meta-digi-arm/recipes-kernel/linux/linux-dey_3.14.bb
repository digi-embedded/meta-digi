# Copyright (C) 2015 Digi International

require recipes-kernel/linux/linux-dey.inc
require recipes-kernel/linux/linux-dtb.inc

inherit fsl-vivante-kernel-driver-handler

SRCBRANCH = "v3.14/dey-2.0/maint"
SRCREV = "e2da450994b1dcc4b187fea9e258d993ef25fc56"

COMPATIBLE_MACHINE = "(ccimx6$)"
