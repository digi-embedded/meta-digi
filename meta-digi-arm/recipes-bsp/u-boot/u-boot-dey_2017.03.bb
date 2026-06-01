# Copyright (C) 2018-2023, Digi International Inc.

require u-boot-dey.inc

SRCBRANCH = "v2017.03/maint"
SRCREV = "76d748841eb363555ff67a7160b7bc8d0f51acb8"

# Disable support to initial environment for U-Boot
UBOOT_INITIAL_ENV = ""

COMPATIBLE_MACHINE = "(ccimx6$|ccimx8x)"
