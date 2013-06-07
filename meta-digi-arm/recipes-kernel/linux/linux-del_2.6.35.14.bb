# Copyright (C) 2012 Digi International

include linux-del.inc

PR = "${DISTRO}.${INC_PR}.0"

SRCREV_external = "c3f3fa8f819f83e77fc0e0c339f12a6fe2a52fef"
SRCREV_internal = "d615e9350f4d14e8a0177ff4a3da8639046657eb"
SRCREV = "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${SRCREV_internal}', '${SRCREV_external}', d)}"

LOCALVERSION_mx5 = "mx5"
LOCALVERSION_mxs = "mxs"
LOCALVERSION_cpx2_mxs = "mxs+gateways"

SRC_URI_external = "${DIGI_GITHUB_GIT}/yocto-linux.git;protocol=git"
SRC_URI_internal = "${DIGI_LOG_GIT}linux-2.6.git;protocol=git"
SRC_URI = "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${SRC_URI_internal}', '${SRC_URI_external}', d)}"
SRC_URI += " \
    file://defconfig \
    ${KERNEL_CFG_FRAGS} \
"
SRC_URI_append_cpx2 = " \
    file://cpx2-Remove-incorrect-pin-configurations.patch \
    file://cpx2e-Setup-mmc0-for-the-wireless-interface.patch \
"
S = "${WORKDIR}/git"

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

FILES_kernel-image += "/boot/config*"

COMPATIBLE_MACHINE = "(mxs|mx5)"
