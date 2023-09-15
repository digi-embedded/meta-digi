# Copyright (C) 2020-2022 Digi International

require u-boot-dey.inc
LIC_FILES_CHKSUM = "file://Licenses/README;md5=30503fd321432fc713238f582193b78e"

DEPENDS += "flex-native bison-native"

SRCBRANCH = "v2020.04/maint"
SRCREV = "6e10827c66bdca9b3b7f55382534d157eebd28fb"

COMPATIBLE_MACHINE = "(ccimx8x|ccimx8m|ccimx6ul)"
