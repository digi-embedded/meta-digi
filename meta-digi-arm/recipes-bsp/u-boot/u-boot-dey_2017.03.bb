# Copyright (C) 2018-2023 Digi International

require u-boot-dey.inc

SRCBRANCH = "v2017.03/maint"
SRCREV = "de9a9d0744ca8d88c1e1bcfa5fa3c43375d4e9da"

# Disable support to initial environment for U-Boot
UBOOT_INITIAL_ENV = ""

COMPATIBLE_MACHINE = "(ccimx6$|ccimx8x)"
