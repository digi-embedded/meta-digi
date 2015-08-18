# Copyright (C) 2015 Digi International

require recipes-kernel/linux/linux-dey.inc
require recipes-kernel/linux/linux-dtb.inc

DEPENDS += "lzop-native bc-native"

# Internal repo branch
SRCBRANCH = "v3.14/master"

SRCREV_external = ""
SRCREV_internal = "1b9c43b31c5d6617ad7797ac2fbf80b8a1fa317e"
SRCREV = "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${SRCREV_internal}', '${SRCREV_external}', d)}"

COMPATIBLE_MACHINE = "(ccimx6)"
