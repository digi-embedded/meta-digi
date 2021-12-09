# Copyright (C) 2018-2021 Digi International

require digi-u-boot.inc

SRCBRANCH = "v2017.03/maint"
SRCREV = "f9055c2439feb717fac5f6266094dbb77fe45992"

# Disable support to initial environment for U-Boot
UBOOT_INITIAL_ENV = ""

COMPATIBLE_MACHINE = "(ccimx6$|ccimx6ul|ccimx8x)"
