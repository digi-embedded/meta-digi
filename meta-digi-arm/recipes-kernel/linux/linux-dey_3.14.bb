# Copyright (C) 2015 Digi International

require recipes-kernel/linux/linux-dey.inc
require recipes-kernel/linux/linux-dtb.inc

inherit fsl-vivante-kernel-driver-handler

# Internal repo branch
SRCBRANCH = "v3.14/dey-2.0/maint"

SRCREV_external = ""
SRCREV_internal = "f14a30884de4222d84ea25d0d0a0ec5582e204d0"
SRCREV = "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${SRCREV_internal}', '${SRCREV_external}', d)}"

COMPATIBLE_MACHINE = "(ccimx6)"
