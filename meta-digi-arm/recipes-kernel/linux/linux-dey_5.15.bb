# Copyright (C) 2022,2023 Digi International

require recipes-kernel/linux/linux-dey.inc

SRCBRANCH = "v5.15.71/nxp/master"
SRCBRANCH:stm32mpcommon = "v5.15.67/stm/master"
SRCREV = "${AUTOREV}"
SRCREV:stm32mpcommon = "${AUTOREV}"

COMPATIBLE_MACHINE = "(ccimx6ul|ccimx8m|ccimx6|ccmp1|ccimx8x)"
