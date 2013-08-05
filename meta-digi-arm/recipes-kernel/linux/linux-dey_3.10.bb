# Copyright (C) 2012 Digi International

UBOOT_ENTRYPOINT  = "0x40008000"

require recipes-kernel/linux/linux-dtb.inc

include linux-dey.inc
include linux-dey-rev_${PV}.inc

PR = "${DISTRO}.${INC_PR}.0"

LOCALVERSION_mxs = "mxs"
LOCALVERSION_cpx2_mxs = "mxs+gateways"

# Preferably configure kernel through device tree.
KERNEL_CFG_FRAGS ?= ""

SRC_URI += " \
    file://defconfig \
    ${KERNEL_CFG_FRAGS} \
"

S = "${WORKDIR}/git"

KERNEL_DEVICETREE = "${S}/arch/arm/boot/dts/${DTSNAME}.dts"
KERNEL_EXTRA_ARGS = "LOADADDR=${UBOOT_LOADADDRESS}"

FILES_kernel-image += "/boot/config*"

COMPATIBLE_MACHINE = "(mxs)"
