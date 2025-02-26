# Copyright (C) 2024,2025 Digi International Inc.

require recipes-kernel/linux/linux-dey.inc

SRCBRANCH = "v6.6/nxp/dey-5.0/maint"
SRCBRANCH:stm32mp2common = "v6.6/stm/dey-5.0/maint"
SRCREV = "87cedd7e8231fee5992dc8d27b6c1448dbfcb1ac"
SRCREV:stm32mp2common = "ff0c2b76588b0cf61cf12b6ebbb929ac84da9ebe"

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
