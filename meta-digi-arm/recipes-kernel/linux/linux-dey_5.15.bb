# Copyright (C) 2022,2023 Digi International

require recipes-kernel/linux/linux-dey.inc

SRCBRANCH = "v5.15/nxp/dey-4.0/maint"
SRCBRANCH:stm32mpcommon = "v5.15/stm/dey-4.0/maint"
SRCREV = "${AUTOREV}"
SRCREV:stm32mpcommon = "${AUTOREV}"

COMPATIBLE_MACHINE = "(ccimx6ul|ccimx8m|ccimx6|ccmp1|ccimx8x)"
