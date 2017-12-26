# Copyright (C) 2015-2018 Digi International

require recipes-kernel/linux/linux-dey.inc

inherit fsl-vivante-kernel-driver-handler

SRCBRANCH = "v3.14/master"
SRCREV = "${AUTOREV}"

COMPATIBLE_MACHINE = "(ccimx6sbc)"
