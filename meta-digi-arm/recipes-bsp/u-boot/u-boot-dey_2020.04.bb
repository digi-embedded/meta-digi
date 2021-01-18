# Copyright (C) 2020 Digi International

require digi-u-boot.inc
LIC_FILES_CHKSUM = "file://Licenses/README;md5=30503fd321432fc713238f582193b78e"

DEPENDS += "flex-native bison-native"

SRCBRANCH = "v2020.04/maint"
SRCREV = "e135e76d1472fece5b280a2f960aec63e324d8e1"

COMPATIBLE_MACHINE = "(ccimx8x|ccimx8m|ccimx6ul)"
