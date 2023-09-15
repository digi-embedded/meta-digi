# Copyright (C) 2023 Digi International

require recipes-kernel/linux/linux-dey.inc

SRCBRANCH = "v6.1/nxp/dey-4.0/maint"
SRCREV = "f6f5d5036faf7dd400415f93b27dae9fa6f47b67"

# Blacklist btnxpuart module. It will be managed by the bluetooth-init script
KERNEL_MODULE_PROBECONF += "btnxpuart"
module_conf_btnxpuart = "blacklist btnxpuart"

COMPATIBLE_MACHINE = "(ccimx93)"
