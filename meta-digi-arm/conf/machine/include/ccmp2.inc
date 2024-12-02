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

# Set metadata settings
DEVICE_PARTUUID_FIP_A = "1c606ef5-f1ac-43b9-9bb5-d5c578580b6b"
DEVICE_PARTUUID_FIP_B = "c7d8648b-76f7-4e2b-b829-e95a83cc7b32"
TF_A_METADATA_TOOL_ARGS ?= "-g -i 1 -b 2 -v ${TF_A_METADATA_TOOL_VERSION} ${DEVICE_PARTUUID_FWU_MDATA},${DEVICE_TYPEUUID_FIP},${DEVICE_PARTUUID_FIP_A},${DEVICE_PARTUUID_FIP_B}"

# =========================================================================
# optee-os
# =========================================================================
PREFERRED_PROVIDER_virtual/optee-os = "optee-os-stm32mp"

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

# Disable by default programmer support:
STM32MP_DEVICETREE_PROGRAMMER_ENABLE ?= "0"

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

# Wic files
WKS_FILE_DEPENDS ?= " \
    virtual/bootloader \
    virtual/trusted-firmware-a \
    ${@bb.utils.contains('BOOTSCHEME_LABELS', 'optee', 'virtual/optee-os', '', d)} \
"

# Disable the generation of flashlayout files
ENABLE_FLASHLAYOUT_CONFIG = "0"
