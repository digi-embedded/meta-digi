# Copyright (C) 2024, Digi International Inc.

require recipes-kernel/linux/linux-dey.inc

SRCBRANCH = "v6.6/nxp/dey-4.0/maint"
SRCREV = "${AUTOREV}"

# Blacklist btnxpuart module. It will be managed by the bluetooth-init script
KERNEL_MODULE_PROBECONF += "btnxpuart"
module_conf_btnxpuart = "blacklist btnxpuart"

COMPATIBLE_MACHINE = "(ccimx91)"
