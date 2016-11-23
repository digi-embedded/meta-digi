# Copyright (C) 2015 Digi International

require recipes-kernel/linux/linux-dey.inc
require recipes-kernel/linux/linux-dtb.inc

inherit fsl-vivante-kernel-driver-handler

SRCBRANCH = "v3.14/dey-2.0/maint"
SRCREV = "e4774bf569b2878839570c9cce71d5165269c1f1"

COMPATIBLE_MACHINE = "(ccimx6$)"
