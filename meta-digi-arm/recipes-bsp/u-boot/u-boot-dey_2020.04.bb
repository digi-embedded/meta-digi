# Copyright (C) 2020-2021 Digi International

require digi-u-boot.inc
LIC_FILES_CHKSUM = "file://Licenses/README;md5=30503fd321432fc713238f582193b78e"

DEPENDS += "flex-native bison-native"

SRCBRANCH = "v2020.04/master"
SRCREV = "dc69c010012356140c816a02b3a3e39088000c0e"

COMPATIBLE_MACHINE = "(ccimx8x|ccimx8m|ccimx6ul)"
