# Copyright (C) 2023 Digi International

require recipes-kernel/linux/linux-dey.inc

SRCBRANCH = "v6.1/nxp/dey-4.0/maint"
SRCREV = "e6c0189b4eefb37def8558d238103023df167a7a"

# Blacklist btnxpuart module. It will be managed by the bluetooth-init script
KERNEL_MODULE_PROBECONF += "btnxpuart"
module_conf_btnxpuart = "blacklist btnxpuart"

COMPATIBLE_MACHINE = "(ccimx93)"
