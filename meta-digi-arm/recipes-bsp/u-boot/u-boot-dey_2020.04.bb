# Copyright (C) 2020-2024, Digi International Inc.

require u-boot-dey.inc
LIC_FILES_CHKSUM = "file://Licenses/README;md5=30503fd321432fc713238f582193b78e"

DEPENDS += "flex-native bison-native"

SRCBRANCH = "v2020.04/master"
SRCREV = "${AUTOREV}"

COMPATIBLE_MACHINE = "(ccimx8m)"
