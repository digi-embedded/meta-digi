# Copyright (C) 2020-2022 Digi International

require digi-u-boot.inc
LIC_FILES_CHKSUM = "file://Licenses/README;md5=30503fd321432fc713238f582193b78e"

DEPENDS += "flex-native bison-native"

SRCBRANCH = "v2020.04/maint"
SRCREV = "ee49926359a70ce04340d80e291b7d9854eb4f9b"

COMPATIBLE_MACHINE = "(ccimx8x|ccimx8m|ccimx6ul)"
