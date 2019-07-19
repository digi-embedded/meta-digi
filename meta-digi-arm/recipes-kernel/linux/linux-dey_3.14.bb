# Copyright (C) 2015 Digi International

require recipes-kernel/linux/linux-dey.inc
require recipes-kernel/linux/linux-dtb.inc

inherit fsl-vivante-kernel-driver-handler

SRC_URI_append_ccimx6 = " \
    file://0001-mmc-remove-check-for-max-EXT_CSD_REV.patch \
"

SRCBRANCH = "v3.14/dey-2.0/maint"
SRCREV = "2585087f7f9cfa69f8c2e9d9e1465e40f132161c"

COMPATIBLE_MACHINE = "(ccimx6$)"
