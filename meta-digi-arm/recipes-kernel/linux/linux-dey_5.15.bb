# Copyright (C) 2022,2023 Digi International

require recipes-kernel/linux/linux-dey.inc

SRCBRANCH = "v5.15/nxp/dey-4.0/maint"
SRCBRANCH:stm32mpcommon = "v5.15/stm/dey-4.0/maint"
SRCREV = "93a4ac0217558b2c56839472f853f6c25dccdff1"
SRCREV:stm32mpcommon = "8487a33b67f387a30382f58b846651eb9035ce50"

COMPATIBLE_MACHINE = "(ccimx6ul|ccimx8m|ccimx6|ccmp1|ccimx8x)"
