# Copyright (C) 2022 Digi International

require u-boot-dey.inc
LIC_FILES_CHKSUM = "file://Licenses/README;md5=5a7450c57ffe5ae63fd732446b988025"

DEPENDS += "flex-native bison-native"
DEPENDS += "python3-setuptools-native"

SRCBRANCH = "v2021.10/maint"
SRCREV = "74a98a8a66bea87aaaaa3cc744c01ab18114028c"

COMPATIBLE_MACHINE = "(ccmp1)"
