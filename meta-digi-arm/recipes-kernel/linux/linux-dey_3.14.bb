# Copyright (C) 2015 Digi International

require recipes-kernel/linux/linux-dey.inc
require recipes-kernel/linux/linux-dtb.inc

inherit fsl-vivante-kernel-driver-handler

# Internal repo branch
SRCBRANCH = "v3.14/master"

SRCREV_external = ""
SRCREV_internal = "${AUTOREV}"
SRCREV = "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${SRCREV_internal}', '${SRCREV_external}', d)}"

COMPATIBLE_MACHINE = "(ccimx6)"
