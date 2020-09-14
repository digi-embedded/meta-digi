#@TYPE: Machine
#@NAME: ConnectCore 6 module.
#@DESCRIPTION: Machine configuration for Digi's ConnectCore 6 module.

DIGI_FAMILY = "ccimx6"
MACHINEOVERRIDES =. "mx6:mx6dl:mx6q:${DIGI_FAMILY}:"

include conf/machine/include/imx-digi-base.inc
include conf/machine/include/tune-cortexa9.inc

# Platform u-boot settings
UBOOT_PREFIX = "u-boot"
UBOOT_SUFFIX = "imx"

# Linux kernel configuration
KERNEL_DEFCONFIG ?= "arch/arm/configs/ccimx6sbc_defconfig"

STORAGE_MEDIA = "mmc"

# Wireless external module
WIRELESS_MODULE ?= ""

# Firmware
MACHINE_FIRMWARE ?= "firmware-imx-sdma"
MACHINE_FIRMWARE_append_mx6q = " firmware-imx-vpu-imx6q"
MACHINE_FIRMWARE_append_mx6dl = " firmware-imx-vpu-imx6d"

MACHINE_EXTRA_RDEPENDS += " \
    e2fsprogs-mke2fs \
    e2fsprogs-resize2fs \
    parted \
"

MACHINE_EXTRA_RRECOMMENDS += " \
    ${MACHINE_FIRMWARE} \
    ${WIRELESS_MODULE} \
    ${@bb.utils.contains('COMBINED_FEATURES', 'alsa', 'imx-alsa-plugins', '', d)} \
"

MACHINE_FEATURES += "accel-graphics accel-video wifi bluetooth pci"

# TrustFence
TRUSTFENCE_SIGN_MODE = "HAB"

# Adding 'wayland' along with 'x11' enables the xwayland backend
DISTRO_FEATURES_append = " wayland"
