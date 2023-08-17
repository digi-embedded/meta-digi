#@TYPE: Machine
#@NAME: ConnectCore 8X module.
#@DESCRIPTION: Machine configuration for Digi's ConnectCore 8X module.

DIGI_SOM = "ccimx8x"
DIGI_FAMILY = "ccimx8x"
MACHINEOVERRIDES =. "mx8:mx8x:mx8qxp:${DIGI_FAMILY}:${DIGI_SOM}:"

include conf/machine/include/imx-digi-base.inc
include conf/machine/include/arm/armv8a/tune-cortexa35.inc

# Platform u-boot settings
UBOOT_PREFIX = "imx-boot"
UBOOT_SUFFIX = "bin"

# The bootloader image that gets flashed consists of U-Boot and several fw binaries
EXTRA_IMAGEDEPENDS += "imx-boot"
BOOTLOADER_IMAGE_RECIPE = "imx-boot"
# BOOTLOADER_SEEK_USERDATA is 33 for step A of the silicon and 32 for step B
BOOTLOADER_SEEK_USERDATA = "32"
# BOOTLOADER_SEEK_BOOT is 32 for step B of silicon and 0 for step C
BOOTLOADER_SEEK_BOOT ?= "0"
BOOTABLE_FILENAME = "${UBOOT_PREFIX}-${MACHINE}.bin"
SDIMG_BOOTLOADER = "${DEPLOY_DIR_IMAGE}/${BOOTABLE_FILENAME}"

# The original variable used by NXP to store the mkimage "REV" parameter,
# REV_OPTION, has a format of "REV=X0". Since we are interested in building
# images for multiple revisions and using the revision in the imx-boot filename,
# use a new variable for our redefined compile, install and deploy functions in
# the imx-boot recipe.
SOC_REVISIONS = "B0 C0"

ATF_PLATFORM = "imx8qx"

# Linux kernel configuration
KERNEL_DEFCONFIG ?= "arch/arm64/configs/ccimx8_defconfig"

STORAGE_MEDIA = "mmc"

# Wireless external module
WIRELESS_MODULE ?= ""
WIRELESS_MODULE:append = " ${@oe.utils.conditional('HAVE_WIFI', '1', 'kernel-module-qualcomm', '', d)}"
HAS_WIFI_VIRTWLANS = "true"

# Machine firmware
MACHINE_FIRMWARE:append = " ${@oe.utils.conditional('HAVE_BT', '1', 'firmware-qualcomm-qca6574-bt', '', d)}"
MACHINE_FIRMWARE:append = " ${@oe.utils.conditional('HAVE_WIFI', '1', 'firmware-qualcomm-qca6574-wifi', '', d)}"

# For the ccimx8x, we generate images for both the B0 and the C0 revisions of
# the i.MX8X silicon, so deploy both versions of the firmware.
SECO_FIRMWARE_NAME:ccimx8x = "mx8qx*0-ahab-container.img"

MACHINE_EXTRA_RDEPENDS += " \
    e2fsprogs-mke2fs \
    e2fsprogs-resize2fs \
    parted \
    xbee-init \
"

MACHINE_EXTRA_RRECOMMENDS += "${WIRELESS_MODULE}"

MACHINE_FEATURES += "accel-graphics wifi bluetooth cryptochip pci mca"

# TrustFence
TRUSTFENCE_SIGN_MODE = "AHAB"

IMXBOOT_TARGETS = "flash_spl"

IMX_BOOT_SOC_TARGET = "iMX8QX"
IMX_BOOT_SEEK = "32"

IMX_EXTRA_FIRMWARE:mx8x-generic-bsp  = "digi-sc-firmware imx-seco"

# SWUpdate sw-description configuration
BOOTFS_EXT ?= ".boot.vfat"
ROOTFS_EXT ?= '${@bb.utils.contains("IMAGE_FEATURES", "read-only-rootfs", ".squashfs", ".ext4.gz", d)}'

BOOT_DEV_NAME ?= "/dev/mmcblk0p1"
ROOTFS_DEV_NAME ?= "/dev/mmcblk0p3"
ROOTFS_ENC_DEV = "/dev/mapper/cryptrootfs"
ROOTFS_DEV_NAME_FINAL = "${@oe.utils.ifelse(d.getVar('TRUSTFENCE_ENCRYPT_ROOTFS') == '1', '${ROOTFS_ENC_DEV}', '${ROOTFS_DEV_NAME}')}"

