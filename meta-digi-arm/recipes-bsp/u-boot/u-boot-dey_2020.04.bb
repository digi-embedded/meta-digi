# Copyright (C) 2020-2023, Digi International Inc.

require u-boot-dey.inc
LIC_FILES_CHKSUM = "file://Licenses/README;md5=30503fd321432fc713238f582193b78e"

DEPENDS += "flex-native bison-native"

SRCBRANCH = "v2020.04/maint"
SRCREV = "c710a20ba35189e0e0fbefaafba9ac86e360919b"

COMPATIBLE_MACHINE = "(ccimx6ul|ccimx8m|ccimx8x)"
