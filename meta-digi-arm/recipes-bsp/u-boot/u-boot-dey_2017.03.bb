# Copyright (C) 2018-2023, Digi International Inc.

require u-boot-dey.inc

SRCBRANCH = "v2017.03/maint"
SRCREV = "${AUTOREV}"

# Disable support to initial environment for U-Boot
UBOOT_INITIAL_ENV = ""

COMPATIBLE_MACHINE = "(ccimx6$|ccimx8x)"
