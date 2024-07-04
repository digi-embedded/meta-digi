#@TYPE: Machine
#@NAME: ConnectCore for STM32MP2.
#@DESCRIPTION: Machine configuration for Digi's ConnectCore MP2.

include conf/machine/include/st-machine-common-stm32mp.inc
include conf/machine/include/digi-defaults.inc

DIGI_FAMILY = "ccmp2"

# Platform u-boot settings
UBOOT_PREFIX = "fip"
UBOOT_SUFFIX = "bin"
# Set U-Boot FIP settings
FIP_UBOOT_DTB = "u-boot"

# =========================================================================
# trusted-firmware-a
# =========================================================================
PREFERRED_PROVIDER_virtual/trusted-firmware-a = "tf-a-stm32mp"
PREFERRED_VERSION_tf-a-stm32mp ?= "v2.8%"

# Configure use of BL31
FIP_BL31_ENABLE = "1"

# =========================================================================
# optee-os
# =========================================================================
PREFERRED_PROVIDER_virtual/optee-os = "optee-os-stm32mp"
PREFERRED_VERSION_optee-os-stm32mp ?= "3.19%"

PREFERRED_VERSION_gcnano-driver-stm32mp ?= "6.4.15%"
PREFERRED_VERSION_gcnano-userland-multi-binary-stm32mp ?= "6.4.15%"

PREFERRED_VERSION_nativesdk-tf-a-tools ?= "v2.8%"
PREFERRED_VERSION_tf-a-tools-native ?= "v2.8%"

# =========================================================================
# Kernel
# =========================================================================
KERNEL_IMAGETYPE = "${@bb.utils.contains('TRUSTFENCE_FIT_IMG', '1', 'fitImage', 'Image.gz', d)}"
KERNEL_CLASSES = "${@bb.utils.contains('TRUSTFENCE_FIT_IMG', '1', 'kernel-fitimage', 'kernel-uimage', d)}"
KERNEL_ALT_IMAGETYPE = "vmlinux"
KERNEL_DEFCONFIG ?= "arch/arm64/configs/ccmp2_defconfig"

STORAGE_MEDIA = "mmc"

# Enable STM specific features
DEY_SOC_VENDOR = "STM"

# SWUpdate sw-description configuration
BOOTFS_EXT ?= ".boot.vfat"
ROOTFS_EXT ?= '${@bb.utils.contains("IMAGE_FEATURES", "read-only-rootfs", ".squashfs", ".ext4.gz", d)}'

BOOT_DEV_NAME ?= "/dev/mmcblk0p6"
ROOTFS_DEV_NAME ?= "/dev/mmcblk0p8"
ROOTFS_ENC_DEV = "/dev/mapper/cryptrootfs"
ROOTFS_DEV_NAME_FINAL = "${@oe.utils.ifelse(d.getVar('TRUSTFENCE_ENCRYPT_ROOTFS') == '1', '${ROOTFS_ENC_DEV}', '${ROOTFS_DEV_NAME}')}"

# List of supported boot devices
BOOTDEVICE_LABELS ?= "emmc"
# Supported boot schemes
BOOTSCHEME_LABELS ?= "optee"

MACHINE_EXTRA_RRECOMMENDS:remove = "kernel-imagebootfs"

MACHINE_EXTRA_RDEPENDS += " \
    e2fsprogs-mke2fs \
    e2fsprogs-resize2fs \
    mtd-utils-ubifs \
    parted \
    xbee-init \
"

# =========================================================================
# Signing configuration
# =========================================================================
ENCRYPT_ENABLE ?= "0"
ENCRYPT_SUFFIX ?= "_Encrypted"

SIGN_ENABLE ?= "0"
SIGN_SUFFIX ?= "_Signed"

# =========================================================================
# Debug trace
# =========================================================================
# activate/deactivate the debug and trace on boot stage
ST_DEBUG_TRACE = "0"
