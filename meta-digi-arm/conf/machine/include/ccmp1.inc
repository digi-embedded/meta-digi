#@TYPE: Machine
#@NAME: ConnectCore for STM32MP1.
#@DESCRIPTION: Machine configuration for Digi's ConnectCore MP1.

include conf/machine/include/st-machine-common-stm32mp.inc
include conf/machine/include/arm/armv7a/tune-cortexa7.inc
include conf/machine/include/digi-defaults.inc

DIGI_FAMILY = "ccmp1"

# Platform u-boot settings
UBOOT_PREFIX = "u-boot"
UBOOT_SUFFIX = "bin"

STORAGE_MEDIA = "mtd"

# Enable STM specific features
DEY_BUILD_PLATFORM = "STM"

# SWUpdate sw-description configuration
BOOTFS_EXT ?= ".boot.ubifs"
ROOTFS_EXT ?= ".ubifs"

BOOT_DEV_NAME ?= "linux"
ROOTFS_DEV_NAME ?= "rootfs"
ROOTFS_ENC_DEV = "${ROOTFS_DEV_NAME}"
ROOTFS_DEV_NAME_FINAL = "${@oe.utils.ifelse(d.getVar('TRUSTFENCE_ENCRYPT_ROOTFS', True) == '1', '${ROOTFS_ENC_DEV}', '${ROOTFS_DEV_NAME}')}"

UBOOT_DEV_NAME ?= "/dev/mtd"

# Extra udev rules
MACHINE_EXTRA_RRECOMMENDS += "udev-extraconf"

# Image FS types
IMAGE_FSTYPES:remove = "ext4 tar.xz"

# List of supported boot devices
BOOTDEVICE_LABELS ?= "sdcard"
