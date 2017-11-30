# Copyright (C) 2015-2017 Digi International

require recipes-kernel/linux/linux-dey.inc
require recipes-kernel/linux/linux-dtb.inc

inherit fsl-vivante-kernel-driver-handler

SRCBRANCH = "v3.14/dey-2.0/maint"
SRCREV = "53e6f29fc7ea7b36f2efed731853c16b4bba39fb"

COMPATIBLE_MACHINE = "(ccimx6sbc)"
