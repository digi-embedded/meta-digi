# Copyright (C) 2012 Digi International

require recipes-kernel/linux/linux-dey.inc

# Internal repo branch
SRCBRANCH = "v2.6.35/dey-1.4/maint"

SRCREV_external = ""
SRCREV_internal = "${AUTOREV}"
SRCREV = "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${SRCREV_internal}', '${SRCREV_external}', d)}"

KERNEL_CFG_FRAGS = " \
    file://config-camera-module.cfg \
    file://config-sahara-module.cfg \
    ${@base_contains('MACHINE_FEATURES', 'accelerometer', 'file://config-accel-module.cfg', '', d)} \
    ${@base_contains('MACHINE_FEATURES', 'ext-eth', 'file://config-ext-eth-module.cfg', '', d)} \
    ${@base_contains('MACHINE_FEATURES', 'wifi', 'file://config-wireless-redpine.cfg', '', d)} \
"
KERNEL_CFG_FRAGS_append_ccimx51js = " file://config-battery-module.cfg"

SRC_URI += "${KERNEL_CFG_FRAGS}"

# Override the do_configure function to add the kernel fragments
do_configure() {
	for i in $(echo ${WORKDIR}/*.cfg); do
		[ "${i}" = "${WORKDIR}/*.cfg" ] && continue
		cat ${i} >> ${B}/.config
	done
	kernel_do_configure
}

COMPATIBLE_MACHINE = "(ccimx5)"
