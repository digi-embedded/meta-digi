# Copyright (C) 2019 Digi International

require digi-u-boot.inc
DEPENDS += "flex-native bison-native"

SRCBRANCH = "v2019.04/master"
SRCREV = "${AUTOREV}"

COMPATIBLE_MACHINE = "(ccimx8x)"