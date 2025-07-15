# Copyright (C) 2024,2025, Digi International Inc.

require recipes-kernel/linux/linux-dey.inc

SRCBRANCH = "v6.6/nxp/dey-5.0/maint"
SRCBRANCH:stm32mpcommon = "v6.6/stm/dey-5.0/maint"
SRCREV = "${AUTOREV}"
SRCREV:stm32mpcommon = "${AUTOREV}"

# Define RT patches per machine
RT_FILES:use-nxp-bsp = " \
    file://0001-add-RT-support-based-on-latest-linux_6.6.36.patch \
    file://fragment-nxp-rt.config \
"
RT_FILES:stm32mpcommon = " \
    file://0010-Rebase-on-v6.6.48-rt40.patch \
    file://0011-v6.6-stm32mp-rt-r1.patch \
    file://fragment-08-deactivate-rng.config \
"
SRC_URI:append = " \
    ${@bb.utils.contains('DISTRO_FEATURES', 'rt', '${RT_FILES}', '', d)} \
"

# Define RT config fragments per machine
RT_CONFIG_FRAGS:use-nxp-bsp = " ${WORKDIR}/fragment-nxp-rt.config"
RT_CONFIG_FRAGS:stm32mpcommon = " \
    ${S}/arch/arm64/configs/fragment-07-rt.config \
    ${S}/arch/arm64/configs/fragment-07-rt-sysvinit.config \
    ${WORKDIR}/fragment-08-deactivate-rng.config \
"
KERNEL_CONFIG_FRAGMENTS:append = " \
    ${@bb.utils.contains('DISTRO_FEATURES', 'rt', '${RT_CONFIG_FRAGS}', '', d)} \
"

# Blacklist btnxpuart module. It will be managed by the bluetooth-init script
KERNEL_MODULE_PROBECONF:ccimx9 += "btnxpuart"
module_conf_btnxpuart:ccimx9 = "blacklist btnxpuart"

# ---------------------------------------------------------------------
# stub for devicetree which are located on digi directory
do_compile:append:stm32mpcommon() {
    if [ -d "${B}/arch/${ARCH}/boot/dts/digi" ]; then
        for dtbf in ${KERNEL_DEVICETREE}; do
            install -m 0644 "${B}/arch/${ARCH}/boot/dts/digi/${dtbf}" "${B}/arch/${ARCH}/boot/dts/"
        done
    fi
}

# Blacklist hci_uart module. It will be managed by the bluetooth-init script
KERNEL_MODULE_PROBECONF:ccmp1 += "hci_uart"
module_conf_hci_uart:ccmp1 = "blacklist hci_uart"

do_install:append:stm32mpcommon() {
    if ${@bb.utils.contains('MACHINE_FEATURES','gpu','true','false',d)}; then
        # when ACCEPT_EULA are filled
        install -d ${D}/${sysconfdir}/modprobe.d/
        echo "blacklist etnaviv" > ${D}/${sysconfdir}/modprobe.d/blacklist.conf
    fi
}

FILES:${KERNEL_PACKAGE_NAME}-modules:stm32mpcommon += "${sysconfdir}/modprobe.d"

COMPATIBLE_MACHINE = "(ccimx6ul|ccimx8m|ccimx8x|ccimx9|ccmp2|ccmp1)"
