# Copyright (C) 2024,2025, Digi International Inc.

require recipes-kernel/linux/linux-dey.inc

SRCBRANCH = "v6.6.52/nxp/master"
SRCBRANCH:stm32mpcommon = "v6.6.78/stm/master"
SRCREV = "${AUTOREV}"
SRCREV:stm32mpcommon = "${AUTOREV}"

# Define RT patches per machine
RT_FILES:use-nxp-bsp = " \
    file://0001-add-RT-support-based-on-latest-linux_6.6.36.patch \
    file://fragment-nxp-rt.config \
"
RT_FILES:stm32mpcommon = " \
    file://0010-Rebase-on-v6.6.78-rt51.patch \
    file://0011-v6.6-stm32mp-rt-r2.patch \
    file://fragment-08-deactivate-rng.config \
    file://fragment-10-network-improvment.config \
"
SRC_URI:append = " \
    ${@bb.utils.contains('DISTRO_FEATURES', 'rt', '${RT_FILES}', '', d)} \
"

SRC_URI:append:ccmp15 = " \
    ${@oe.utils.conditional('TRUSTFENCE_COPRO_ENABLED', '1' , 'file://0001-ARM-dts-ccmp15-add-signed-firmware-support-for-RPROC.patch \
                                                               file://0002-remoteproc-stm32_rproc-make-reset-and-hold-boot-opti.patch', '', d)} \
"

SRC_URI:append:ccmp25 = " \
    ${@oe.utils.conditional('TRUSTFENCE_COPRO_ENABLED', '1' , 'file://0001-ARM64-dts-ccmp25-add-signed-firmware-support-for-RPR.patch', '', d)} \
"

# Define RT config fragments per machine
RT_CONFIG_FRAGS:use-nxp-bsp = " ${WORKDIR}/fragment-nxp-rt.config"
RT_CONFIG_FRAGS:stm32mpcommon = " \
    ${S}/arch/arm64/configs/fragment-07-rt.config \
    ${S}/arch/arm64/configs/fragment-07-rt-sysvinit.config \
    ${WORKDIR}/fragment-08-deactivate-rng.config \
    ${WORKDIR}/fragment-10-network-improvment.config \
"
KERNEL_CONFIG_FRAGMENTS:append = " \
    ${@bb.utils.contains('DISTRO_FEATURES', 'rt', '${RT_CONFIG_FRAGS}', '', d)} \
"

# Blacklist BT driver (as module). They will be managed by the bluetooth-init script
KERNEL_MODULE_PROBECONF:ccimx91 += "btnxpuart"
KERNEL_MODULE_PROBECONF:ccimx93 += "btnxpuart"
KERNEL_MODULE_PROBECONF:ccimx95 += "hci_uart"
KERNEL_MODULE_PROBECONF:ccmp1 += "hci_uart"
module_conf_btnxpuart:ccimx91 = "blacklist btnxpuart"
module_conf_btnxpuart:ccimx93 = "blacklist btnxpuart"
module_conf_hci_uart:ccimx95 = "blacklist hci_uart"
module_conf_hci_uart:ccmp1 = "blacklist hci_uart"

# ---------------------------------------------------------------------
# stub for devicetree which are located on digi directory
do_compile:append:stm32mpcommon() {
    if [ -d "${B}/arch/${ARCH}/boot/dts/digi" ]; then
        for dtbf in ${KERNEL_DEVICETREE}; do
            install -m 0644 "${B}/arch/${ARCH}/boot/dts/digi/${dtbf}" "${B}/arch/${ARCH}/boot/dts/"
        done
    fi
}

do_install:append:stm32mpcommon() {
    if ${@bb.utils.contains('MACHINE_FEATURES','gpu','true','false',d)}; then
        # when ACCEPT_EULA are filled
        install -d ${D}/${sysconfdir}/modprobe.d/
        echo "blacklist etnaviv" > ${D}/${sysconfdir}/modprobe.d/blacklist.conf
    fi
}

FILES:${KERNEL_PACKAGE_NAME}-modules:stm32mpcommon += "${sysconfdir}/modprobe.d"

COMPATIBLE_MACHINE = "(ccimx6$|ccimx6ul|ccimx8m|ccimx8x|ccimx9|ccmp2|ccmp1)"
