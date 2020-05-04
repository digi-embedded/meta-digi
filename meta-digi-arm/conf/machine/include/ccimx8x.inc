#@TYPE: Machine
#@NAME: ConnectCore 8X module.
#@DESCRIPTION: Machine configuration for Digi's ConnectCore 8X module.

DIGI_FAMILY = "ccimx8x"
MACHINEOVERRIDES =. "mx8:mx8x:mx8qxp:${DIGI_FAMILY}:"

include conf/machine/include/imx-digi-base.inc
include conf/machine/include/arm/arch-arm64.inc

# Platform u-boot settings
UBOOT_PREFIX = "imx-boot"
UBOOT_SUFFIX = "bin"

# The bootloader image that gets flashed consists of U-Boot and several fw binaries
EXTRA_IMAGEDEPENDS = "imx-boot"
BOOTLOADER_IMAGE_RECIPE = "imx-boot"
# BOOTLOADER_SEEK is 33 for step A of the silicon and 32 for step B
BOOTLOADER_SEEK = "32"
BOOTABLE_FILENAME = "${UBOOT_PREFIX}-${MACHINE}.bin"
SDIMG_BOOTLOADER = "${DEPLOY_DIR_IMAGE}/${BOOTABLE_FILENAME}"

# RAM variants
# This determines the number of different DCD files, and thus the number of
# different U-Boot binaries required.
# Last one is the default (the one the symlinks point at)
RAM_CONFIGS = "1.2GHz_512MB_16bit 1.2GHz_1GB_16bit 1.2GHz_1GB_32bit 1.2GHz_2GB_32bit"

# The original variable used by NXP to store the mkimage "REV" parameter,
# REV_OPTION, has a format of "REV=X0". Since we are interested in building
# images for multiple revisions and using the revision in the imx-boot filename,
# use a new variable for our redefined compile, install and deploy functions in
# the imx-boot recipe.
SOC_REVISIONS = "B0 C0"

# Linux kernel configuration
KERNEL_DEFCONFIG ?= "arch/arm64/configs/ccimx8_defconfig"

STORAGE_MEDIA = "mmc"

# Wireless external module
WIRELESS_MODULE ?= ""
WIRELESS_MODULE_append = " ${@oe.utils.conditional('HAVE_WIFI', '1', 'kernel-module-qualcomm', '', d)}"
HAS_WIFI_VIRTWLANS = "true"

# Firmware
MACHINE_FIRMWARE ?= "firmware-imx-vpu-imx8 firmware-imx-sdma"
MACHINE_FIRMWARE_append = " ${@oe.utils.conditional('HAVE_BT', '1', 'firmware-qualcomm-qca6574-bt', '', d)}"
MACHINE_FIRMWARE_append = " ${@oe.utils.conditional('HAVE_WIFI', '1', 'firmware-qualcomm-qca6574-wifi', '', d)}"

MACHINE_EXTRA_RDEPENDS += " \
    e2fsprogs-mke2fs \
    e2fsprogs-resize2fs \
    parted \
    xbee-init \
"

MACHINE_EXTRA_RRECOMMENDS += " \
    ${MACHINE_FIRMWARE} \
    ${WIRELESS_MODULE} \
"

MACHINE_FEATURES += "accel-graphics accel-video wifi bluetooth cryptochip pci mca"

# AARCH64 doesn't support self-extracting zImage
KERNEL_IMAGETYPE = "Image.gz"

VIRTUAL-RUNTIME_init_manager ?= "systemd"
VIRTUAL-RUNTIME_initscripts ?= "initscripts"

# For i.MX 8 silicon chip revision
MX8_CHIP_REV ?= "B0"
MX8_SOC_VAR ?= "QX"

# TrustFence
TRUSTFENCE_SIGN_MODE = "AHAB"
# TODO: not yet supported
TRUSTFENCE_ENCRYPT_ENVIRONMENT = "0"
# For Trustfence container header RAM locations
RAM_CONTAINER_LOC_BOOT = "0x80280000"
RAM_CONTAINER_LOC_DTB = "0x82000000"
RAM_CONTAINER_LOC_TF = "0x82100000"

# Adding 'wayland' along with 'x11' enables the xwayland backend
# Vulkan is necessary for wayland to build
DISTRO_FEATURES_append = " wayland vulkan systemd pam"
DISTRO_FEATURES_BACKFILL_CONSIDERED = "sysvinit"
