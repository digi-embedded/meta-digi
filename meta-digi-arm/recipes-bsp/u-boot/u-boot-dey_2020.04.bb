# Copyright (C) 2020-2023 Digi International

require u-boot-dey.inc
LIC_FILES_CHKSUM = "file://Licenses/README;md5=30503fd321432fc713238f582193b78e"

DEPENDS += "flex-native bison-native"

SRCBRANCH = "v2020.04/maint"
SRCREV = "d6e1da4a6b0a5407b39e5705ed4e845737c38536"

COMPATIBLE_MACHINE = "(ccimx6ul|ccimx8m|ccimx8x)"
