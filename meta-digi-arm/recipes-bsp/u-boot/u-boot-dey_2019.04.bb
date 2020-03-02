# Copyright (C) 2019 Digi International

require digi-u-boot.inc
LIC_FILES_CHKSUM = "file://Licenses/README;md5=30503fd321432fc713238f582193b78e"

DEPENDS += "flex-native bison-native"

SRCBRANCH = "v2019.04/master"
SRCREV = "${AUTOREV}"

COMPATIBLE_MACHINE = "(ccimx6ul|ccimx8x)"