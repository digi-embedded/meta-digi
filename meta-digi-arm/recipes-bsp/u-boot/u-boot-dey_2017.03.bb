# Copyright (C) 2018-2021 Digi International

require digi-u-boot.inc

SRCBRANCH = "v2017.03/maint"
SRCREV = "f16d125897b243e48d9b78e577eed52d3de1896d"

# Disable support to initial environment for U-Boot
UBOOT_INITIAL_ENV = ""

COMPATIBLE_MACHINE = "(ccimx6$|ccimx6ul|ccimx8x)"
