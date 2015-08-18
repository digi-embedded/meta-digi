# Copyright (C) 2012 Digi International

require recipes-kernel/linux/linux-dey.inc

COMPATIBLE_MACHINE = "(mx5)"

# Internal repo branch
SRCBRANCH = "v2.6.35/dey-1.4/maint"

SRCREV_external = ""
SRCREV_internal = "4c04cc2a9bbcfd17e34cc3507b293907c83e8f0d"
SRCREV = "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${SRCREV_internal}', '${SRCREV_external}', d)}"

KERNEL_CFG_FRAGS ?= ""
KERNEL_CFG_FRAGS_append_mx5 = "file://config-sahara-module.cfg file://config-camera-module.cfg"
KERNEL_CFG_FRAGS_append_mx5 = " ${@base_contains('MACHINE_FEATURES', 'accelerometer', 'file://config-accel-module.cfg', '', d)}"
KERNEL_CFG_FRAGS_append_mx5 = " ${@base_contains('MACHINE_FEATURES', 'ext-eth', 'file://config-ext-eth-module.cfg', '', d)}"
KERNEL_CFG_FRAGS_append_mx5 = " ${@base_contains('MACHINE_FEATURES', 'wifi', 'file://config-wireless-redpine.cfg', '', d)}"
KERNEL_CFG_FRAGS_append_ccimx51js = " file://config-battery-module.cfg"
