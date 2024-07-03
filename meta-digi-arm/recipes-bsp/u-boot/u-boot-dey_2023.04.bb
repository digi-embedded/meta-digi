# Copyright (C) 2023,2024, Digi International Inc.

require u-boot-dey.inc
LIC_FILES_CHKSUM = "file://Licenses/README;md5=2ca5f2c35c8cc335f0a19756634782f1"

DEPENDS += "flex-native bison-native"

SRCBRANCH = "v2023.04/master"
SRCREV = "${AUTOREV}"

COMPATIBLE_MACHINE = "(ccimx9)"
