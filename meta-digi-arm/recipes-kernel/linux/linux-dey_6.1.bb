# Copyright (C) 2023,2024, Digi International Inc.

require recipes-kernel/linux/linux-dey.inc

SRCBRANCH = "v6.1.28/stm/master"
SRCREV = "${AUTOREV}"

SRC_URI:append = " \
    ${@bb.utils.contains('DISTRO_FEATURES', 'tsn', 'file://tsn_conf.cfg', '', d)} \
"

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

COMPATIBLE_MACHINE = "(ccmp2)"
