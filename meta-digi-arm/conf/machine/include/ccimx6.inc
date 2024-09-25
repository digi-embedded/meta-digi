#@TYPE: Machine
#@NAME: ConnectCore 6 module.
#@DESCRIPTION: Machine configuration for Digi's ConnectCore 6 module.

DIGI_FAMILY = "ccimx6"
MACHINEOVERRIDES =. "mx6:mx6dl:mx6q:${DIGI_FAMILY}:${DIGI_SOM}:"

include conf/machine/include/imx-digi-base.inc
include conf/machine/include/arm/armv7a/tune-cortexa9.inc

BOOTLOADER_SEEK_BOOTPART = "1"

# Platform u-boot settings
UBOOT_PREFIX = "u-boot"
UBOOT_SUFFIX = "imx"
BOOTABLE_FILENAME = "${UBOOT_PREFIX}-${MACHINE}.${UBOOT_SUFFIX}"
SDIMG_BOOTLOADER = "${DEPLOY_DIR_IMAGE}/${BOOTABLE_FILENAME}"

# Set InitRAMDisk load address to U-boot initramfs_addr
UBOOT_RD_LOADADDRESS = "0x19000000"

# Linux kernel configuration
KERNEL_DEFCONFIG ?= "arch/arm/configs/ccimx6sbc_defconfig"

STORAGE_MEDIA = "mmc"

# Wireless external module
WIRELESS_MODULE ?= ""

MACHINE_EXTRA_RDEPENDS += " \
    e2fsprogs-mke2fs \
    e2fsprogs-resize2fs \
    parted \
"

MACHINE_EXTRA_RRECOMMENDS += "${WIRELESS_MODULE}"

MACHINE_FEATURES += "accel-graphics wifi bluetooth pci"

# TrustFence
TRUSTFENCE_SIGN_MODE = "HAB"

# Per-machine DISTRO_FEATURES customization
MACHINE_DISTRO_FEATURES_REMOVE = "vulkan"

# SWUpdate sw-description configuration
BOOTFS_EXT ?= ".boot.vfat"
ROOTFS_EXT ?= '${@bb.utils.contains("IMAGE_FEATURES", "read-only-rootfs", ".squashfs", ".ext4.gz", d)}'

BOOT_DEV_NAME ?= "/dev/mmcblk0p1"
ROOTFS_DEV_NAME ?= "/dev/mmcblk0p3"
ROOTFS_ENC_DEV = "/dev/mapper/cryptrootfs"
ROOTFS_DEV_NAME_FINAL = "${@oe.utils.ifelse(d.getVar('TRUSTFENCE_ENCRYPT_ROOTFS') == '1', '${ROOTFS_ENC_DEV}', '${ROOTFS_DEV_NAME}')}"

