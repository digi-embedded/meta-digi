# Copyright (C) 2023-2024 Digi International

require recipes-kernel/linux/linux-dey.inc

SRCBRANCH = "v6.1.55/nxp/master"

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
"

SRCREV = "${AUTOREV}"

# Blacklist btnxpuart module. It will be managed by the bluetooth-init script
KERNEL_MODULE_PROBECONF += "btnxpuart"
module_conf_btnxpuart = "blacklist btnxpuart"

COMPATIBLE_MACHINE = "(ccimx93)"
