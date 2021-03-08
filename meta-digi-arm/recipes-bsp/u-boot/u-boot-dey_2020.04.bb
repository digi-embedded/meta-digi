# Copyright (C) 2020-2021 Digi International

require digi-u-boot.inc
LIC_FILES_CHKSUM = "file://Licenses/README;md5=30503fd321432fc713238f582193b78e"

DEPENDS += "flex-native bison-native"

SRCBRANCH = "v2020.04/maint"
SRCREV = "749d90196c7d28cb6995d9ab5141d97b9079735d"

COMPATIBLE_MACHINE = "(ccimx8x|ccimx8m|ccimx6ul)"
