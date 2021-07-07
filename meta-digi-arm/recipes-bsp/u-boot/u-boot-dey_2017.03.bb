# Copyright (C) 2018-2021 Digi International

require digi-u-boot.inc

SRCBRANCH = "v2017.03/maint"
SRCREV = "31e1721d47b6b045af59fb2dedcb75a1456a8071"

# Disable support to initial environment for U-Boot
UBOOT_INITIAL_ENV = ""

COMPATIBLE_MACHINE = "(ccimx6$|ccimx6ul|ccimx8x)"
