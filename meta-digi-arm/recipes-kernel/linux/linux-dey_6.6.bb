# Copyright (C) 2024,2025 Digi International Inc.

require recipes-kernel/linux/linux-dey.inc

SRCBRANCH = "v6.6/nxp/dey-5.0/maint"
SRCBRANCH:stm32mp2common = "v6.6/stm/dey-5.0/maint"
SRCREV = "${AUTOREV}"
SRCREV:stm32mp2common = "${AUTOREV}"

STM_RT_FILES = " \
	file://0010-Rebase-on-v6.6.48-rt40.patch \
	file://0011-v6.6-stm32mp-rt-r1.patch \
	file://fragment-08-deactivate-rng.config \
"
SRC_URI:append:stm32mpcommon = " \
	${@bb.utils.contains('DISTRO_FEATURES', 'rt', '${STM_RT_FILES}', '', d)} \
"

STM_RT_CONFIG_FRAGS = " \
	${S}/arch/arm64/configs/fragment-07-rt.config \
	${S}/arch/arm64/configs/fragment-07-rt-sysvinit.config \
	${WORKDIR}/fragment-08-deactivate-rng.config \
"
KERNEL_CONFIG_FRAGMENTS:append:stm32mpcommon = " ${@bb.utils.contains('DISTRO_FEATURES', 'rt', '${STM_RT_CONFIG_FRAGS}', '', d)}"

# Blacklist btnxpuart module. It will be managed by the bluetooth-init script
KERNEL_MODULE_PROBECONF += "btnxpuart"
module_conf_btnxpuart = "blacklist btnxpuart"

# ---------------------------------------------------------------------
# stub for devicetree which are located on digi directory
do_install:prepend:ccmp2() {
    if [ -d "${B}/arch/${ARCH}/boot/dts/digi" ]; then
        for dtbf in ${KERNEL_DEVICETREE}; do
            install -m 0644 "${B}/arch/${ARCH}/boot/dts/digi/${dtbf}" "${B}/arch/${ARCH}/boot/dts/"
        done
    fi
}

do_install:append:ccmp2() {
    if ${@bb.utils.contains('MACHINE_FEATURES','gpu','true','false',d)}; then
        # when ACCEPT_EULA are filled
        install -d ${D}/${sysconfdir}/modprobe.d/
        echo "blacklist etnaviv" > ${D}/${sysconfdir}/modprobe.d/blacklist.conf
    fi
}

FILES:${KERNEL_PACKAGE_NAME}-modules:ccmp2 += "${sysconfdir}/modprobe.d"

COMPATIBLE_MACHINE = "(ccimx6ul|ccimx8x|ccimx9|ccmp2)"
