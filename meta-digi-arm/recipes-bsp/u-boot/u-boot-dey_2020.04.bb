# Copyright (C) 2020-2021 Digi International

require digi-u-boot.inc
LIC_FILES_CHKSUM = "file://Licenses/README;md5=30503fd321432fc713238f582193b78e"

DEPENDS += "flex-native bison-native"

SRCBRANCH = "v2020.04/maint"
SRCREV = "425dadfb02c216d8521fff8093a5fb4cf8db7380"

COMPATIBLE_MACHINE = "(ccimx8x|ccimx8m|ccimx6ul)"
