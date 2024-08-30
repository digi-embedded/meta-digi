# Copyright (C) 2023,2024, Digi International Inc.

require recipes-kernel/linux/linux-dey.inc

SRCBRANCH = "v6.1.55/nxp/master"
SRCBRANCH:stm32mp2common = "v6.1.28/stm/master"

# Patch series for RT Kernel
NXP_RT_PATCHES = " \
    file://0001-arch-arm-add-NXP-RT-support.patch \
    file://0002-RT-add-RT-localversion.patch \
    file://0003-arch-arm64-add-NXP-RT-support.patch \
    file://0004-Documentation-add-NXP-RT-support.patch \
    file://0005-include-add-NXP-RT-support.patch \
    file://0006-kernel-add-NXP-RT-support.patch \
    file://0007-drivers-add-NXP-RT-support.patch \
    file://0008-net-add-RT-NXP-support.patch \
    file://0009-init-add-NXP-RT-support.patch \
    file://nxp_rt_conf.cfg \
"

SRC_URI:append = " \
    ${@bb.utils.contains('DISTRO_FEATURES', 'rt', '${NXP_RT_PATCHES}', '', d)} \
    ${@bb.utils.contains('DISTRO_FEATURES', 'tsn', 'file://tsn_conf.cfg', '', d)} \
"

SRCREV = "${AUTOREV}"

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

COMPATIBLE_MACHINE = "(ccimx93|ccmp2)"
