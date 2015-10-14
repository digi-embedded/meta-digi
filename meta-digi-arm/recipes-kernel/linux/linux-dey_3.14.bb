# Copyright (C) 2015 Digi International

require recipes-kernel/linux/linux-dey.inc
require recipes-kernel/linux/linux-dtb.inc

# Internal repo branch
SRCBRANCH = "v3.14/master"

SRCREV_external = ""
SRCREV_internal = "9d8c0db7185570a426c3b5780ff62669ea71265e"
SRCREV = "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${SRCREV_internal}', '${SRCREV_external}', d)}"

COMPATIBLE_MACHINE = "(ccimx6)"
