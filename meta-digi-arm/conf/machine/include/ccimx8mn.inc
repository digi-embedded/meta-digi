#@TYPE: Machine
#@NAME: ConnectCore 8M Nano module.
#@DESCRIPTION: Machine configuration for Digi's ConnectCore 8M Nano module.

DIGI_FAMILY = "ccimx8mn"
MACHINEOVERRIDES =. "mx8:mx8m:mx8mn:ccimx8m:${DIGI_FAMILY}:"

include conf/machine/include/imx-digi-base.inc
include conf/machine/include/arm/arch-arm64.inc

# Platform u-boot settings
UBOOT_PREFIX = "imx-boot"
UBOOT_SUFFIX = "bin"

# The bootloader image that gets flashed consists of U-Boot and several fw binaries
EXTRA_IMAGEDEPENDS = "imx-boot"
BOOTLOADER_IMAGE_RECIPE = "imx-boot"
BOOTLOADER_SEEK = "32"
BOOTABLE_FILENAME = "${UBOOT_PREFIX}-${MACHINE}.bin"
SDIMG_BOOTLOADER = "${DEPLOY_DIR_IMAGE}/${BOOTABLE_FILENAME}"

# Linux kernel configuration
KERNEL_DEFCONFIG ?= "arch/arm64/configs/ccimx8_defconfig"

# Wireless external module
WIRELESS_MODULE ?= ""

# Firmware
MACHINE_FIRMWARE ?= "firmware-imx-sdma firmware-imx-easrc"

MACHINE_EXTRA_RDEPENDS += " \
    e2fsprogs-mke2fs \
    e2fsprogs-resize2fs \
    mca-tool \
    parted \
    u-boot-fw-utils \
    xbee-init \
"

MACHINE_EXTRA_RRECOMMENDS += " \
    ${MACHINE_FIRMWARE} \
    ${WIRELESS_MODULE} \
    cryptodev-module \
    cryptoauthlib \
"

MACHINE_FEATURES += "accel-graphics accel-video wifi bluetooth cryptochip"

# AARCH64 doesn't support self-extracting zImage
KERNEL_IMAGETYPE = "Image.gz"

VIRTUAL-RUNTIME_init_manager ?= "systemd"
VIRTUAL-RUNTIME_initscripts ?= "initscripts"

# Adding 'wayland' along with 'x11' enables the xwayland backend
# Vulkan is necessary for wayland to build
DISTRO_FEATURES_append = " wayland vulkan systemd pam"
DISTRO_FEATURES_BACKFILL_CONSIDERED = "sysvinit"
