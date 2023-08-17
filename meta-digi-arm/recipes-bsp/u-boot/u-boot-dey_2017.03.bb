# Copyright (C) 2018-2021 Digi International

require u-boot-dey.inc

SRCBRANCH = "v2017.03/maint"
SRCREV = "${AUTOREV}"

# Disable support to initial environment for U-Boot
UBOOT_INITIAL_ENV = ""

COMPATIBLE_MACHINE = "(ccimx6$|ccimx8x)"
