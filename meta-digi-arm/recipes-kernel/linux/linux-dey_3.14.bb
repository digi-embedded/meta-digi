# Copyright (C) 2015-2017 Digi International

require recipes-kernel/linux/linux-dey.inc
require recipes-kernel/linux/linux-dtb.inc

inherit fsl-vivante-kernel-driver-handler

SRCBRANCH = "v3.14/dey-2.0/maint"
SRCREV = "359feaf8f7bfdc95735fc54f9e49e5f8b4ff34eb"

COMPATIBLE_MACHINE = "(ccimx6sbc)"
