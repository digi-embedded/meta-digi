# Copyright (C) 2012 Digi International

PR = "${DISTRO}.${INC_PR}.0"

require recipes-kernel/linux/linux-dey.inc

COMPATIBLE_MACHINE = "(mxs|mx5)"

KBRANCH_DEFAULT = "del-5.9/meta-digi"
KBRANCH = "${KBRANCH_DEFAULT}"

SRCREV_external = ""
SRCREV_internal = "4868025cc31b4fec814094053c8a8067bc7b5943"
SRCREV = "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${SRCREV_internal}', '${SRCREV_external}', d)}"

LOCALVERSION_mx5 = "mx5"
LOCALVERSION_mxs = "mxs"

KERNEL_CFG_FRAGS ?= ""
KERNEL_CFG_FRAGS_append_mx5 = "file://config-sahara-module.cfg file://config-camera-module.cfg"
KERNEL_CFG_FRAGS_append_mx5 = " ${@base_contains('MACHINE_FEATURES', 'accelerometer', 'file://config-accel-module.cfg', '', d)}"
KERNEL_CFG_FRAGS_append_mx5 = " ${@base_contains('MACHINE_FEATURES', 'ext-eth', 'file://config-ext-eth-module.cfg', '', d)}"
KERNEL_CFG_FRAGS_append_mx5 = " ${@base_contains('MACHINE_FEATURES', 'wifi', 'file://config-wireless-redpine.cfg', '', d)}"
KERNEL_CFG_FRAGS_append_ccimx51js = " file://config-battery-module.cfg"
KERNEL_CFG_FRAGS_append_mxs = " ${@base_contains('MACHINE_FEATURES', 'wifi', 'file://config-wireless-atheros.cfg', '', d)}"
KERNEL_CFG_FRAGS_append_mxs = " ${@base_contains('MACHINE_FEATURES', 'bluetooth', 'file://config-bluetooth-atheros.cfg', '', d)}"
KERNEL_CFG_FRAGS_append_ccardimx28js = " ${@base_contains('MACHINE_FEATURES', '1-wire', 'file://config-1-wire.cfg', '', d)}"
KERNEL_CFG_FRAGS_append_ccardimx28js = " ${@base_contains('MACHINE_FEATURES', 'ext-eth', 'file://config-ext-eth.cfg', '', d)}"
KERNEL_CFG_FRAGS_append_ccardimx28js = " ${@base_contains('DISTRO_FEATURES', 'x11', 'file://config-fb.cfg file://config-touch.cfg', '', d)}"
KERNEL_CFG_FRAGS_append_ccardimx28js = " ${@base_contains('MACHINE_FEATURES', 'alsa', 'file://config-sound.cfg', '', d)}"
