#@TYPE: Machine
#@NAME: ConnectCore 8X module.
#@DESCRIPTION: Machine configuration for Digi's ConnectCore 8X module.

DIGI_FAMILY = "ccimx8x"
MACHINEOVERRIDES =. "mx8:mx8x:mx8qxp:mx8dxp:mx8dx:${DIGI_FAMILY}:"

include conf/machine/include/imx-digi-base.inc
include conf/machine/include/arm/arch-arm64.inc

# Platform u-boot settings
UBOOT_SUFFIX = "bin"
UBOOT_SYMLINK = "u-boot-${MACHINE}.${UBOOT_SUFFIX}"

# The bootloader image that gets flashed consists of U-Boot and several fw binaries
EXTRA_IMAGEDEPENDS = "imx-boot"

# Linux kernel configuration
KERNEL_DEFCONFIG ?= "arch/arm64/configs/ccimx8x_defconfig"

# Wireless external module
WIRELESS_MODULE ?= ""

# Firmware
MACHINE_FIRMWARE ?= ""

MACHINE_EXTRA_RDEPENDS += " \
    e2fsprogs-mke2fs \
    e2fsprogs-resize2fs \
    parted \
    u-boot-fw-utils \
"

MACHINE_EXTRA_RRECOMMENDS += " \
    ${MACHINE_FIRMWARE} \
    ${WIRELESS_MODULE} \
    cryptodev-module \
"

MACHINE_FEATURES += "accel-graphics accel-video wifi bluetooth cryptochip"

# AARCH64 doesn't support self-extracting zImage
KERNEL_IMAGETYPE = "Image.gz"
