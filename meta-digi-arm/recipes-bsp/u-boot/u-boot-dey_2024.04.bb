# Copyright (C) 2024,2025, Digi International Inc.

require u-boot-dey.inc

LIC_FILES_CHKSUM = "file://Licenses/README;md5=2ca5f2c35c8cc335f0a19756634782f1"

DEPENDS += "flex-native bison-native"

SRCBRANCH = "v2024.04/maint"
SRCREV = "31c334d16837e271911778a0e908b1440361220e"

COMPATIBLE_MACHINE = "(ccimx6$|ccimx6ul|ccimx8m|ccimx8x|ccimx9)"
