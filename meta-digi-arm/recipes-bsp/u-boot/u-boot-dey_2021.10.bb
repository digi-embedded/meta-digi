# Copyright (C) 2022 Digi International

require u-boot-dey.inc
LIC_FILES_CHKSUM = "file://Licenses/README;md5=5a7450c57ffe5ae63fd732446b988025"

DEPENDS += "flex-native bison-native"
DEPENDS += "python3-setuptools-native"

SRCBRANCH = "v2021.10/maint"
SRCREV = "3d06f32fbc20428382c4d4db7cba87e93897bb16"

COMPATIBLE_MACHINE = "(ccmp1)"
