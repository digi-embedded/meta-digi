# Copyright (C) 2020-2023 Digi International

require u-boot-dey.inc
LIC_FILES_CHKSUM = "file://Licenses/README;md5=30503fd321432fc713238f582193b78e"

DEPENDS += "flex-native bison-native"

SRCBRANCH = "v2020.04/maint"
SRCREV = "af77921f513e82add24e54a28a5353b9012fdaf6"

COMPATIBLE_MACHINE = "(ccimx6ul|ccimx8m|ccimx8x)"
