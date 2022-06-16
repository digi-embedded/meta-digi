# Copyright (C) 2020-2021 Digi International

require digi-u-boot.inc
LIC_FILES_CHKSUM = "file://Licenses/README;md5=30503fd321432fc713238f582193b78e"

DEPENDS += "flex-native bison-native"

SRCBRANCH = "v2020.04/maint"
SRCREV = "5742050e966238297c5d45d79b461ba91a80b930"

COMPATIBLE_MACHINE = "(ccimx8x|ccimx8m|ccimx6ul)"
