# Copyright (C) 2024,2025, Digi International Inc.

require u-boot-dey.inc

LIC_FILES_CHKSUM = "file://Licenses/README;md5=2ca5f2c35c8cc335f0a19756634782f1"

DEPENDS += "flex-native bison-native"

SRCBRANCH = "v2024.04/maint"
SRCREV = "dc2bd5d288ca472cb200c4c4e40c098ce14e84d5"

COMPATIBLE_MACHINE = "(ccimx6ul|ccimx8m|ccimx8x|ccimx9)"
