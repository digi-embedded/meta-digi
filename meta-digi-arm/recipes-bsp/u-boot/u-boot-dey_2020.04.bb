# Copyright (C) 2020-2022 Digi International

require u-boot-dey.inc
LIC_FILES_CHKSUM = "file://Licenses/README;md5=30503fd321432fc713238f582193b78e"

DEPENDS += "flex-native bison-native"

SRCBRANCH = "v2020.04/maint"
SRCREV = "5df0daa78474fc9f48bf1584bb18afcd8d1d1769"

COMPATIBLE_MACHINE = "(ccimx8x|ccimx8m|ccimx6ul)"
