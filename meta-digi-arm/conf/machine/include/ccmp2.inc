#@TYPE: Machine
#@NAME: ConnectCore for STM32MP2.
#@DESCRIPTION: Machine configuration for Digi's ConnectCore MP2.

include conf/machine/include/st-machine-common-stm32mp.inc
include conf/machine/include/digi-defaults.inc

DIGI_FAMILY = "ccmp2"

# =========================================================================
# m33copro
# =========================================================================
# Set list Cortex-M33 applicantions
M33COPRO_LIST = ""

# =========================================================================
# U-Boot
# =========================================================================
# Platform u-boot settings
UBOOT_PREFIX = "fip"
UBOOT_SUFFIX = "bin"
# Set U-Boot FIP settings
FIP_UBOOT_DTB = "u-boot"
# Include boot script into the FIT image
UBOOT_ENV = "boot"
# For FIT image usage
UBOOT_ENTRYPOINT = "0x88000000"
UBOOT_DTB_LOADADDRESS = "0x8a000000"
UBOOT_DTBO_LOADADDRESS = "0x8a000000"

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
# OPTEE runtime packages to install
OPTEE_PKGS ??= "optee-client"

# =========================================================================
# Kernel
# =========================================================================
KERNEL_IMAGETYPE = "${@bb.utils.contains('TRUSTFENCE_SIGN_FIT_STM', '1', 'fitImage', 'Image.gz', d)}"
KERNEL_CLASSES = "kernel-fitimage"
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
BOOTDEVICE_LABELS ?= "emmc usb"
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
# alsa
# =========================================================================
ALSA_LIST:stm32mp2common = ""

# Image FS types (set by meta-st-stm32mp layer)
# tar.xz is omitted here because it's already added by ST layer
IMAGE_FSTYPES += '${@bb.utils.contains("IMAGE_FEATURES", "read-only-rootfs", \
                                       "boot.vfat recovery.vfat squashfs", \
                                       "boot.vfat recovery.vfat ext4.gz", d)}'
IMAGE_FSTYPES:remove = "ext4"

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
# Reduce optee log level
ST_OPTEE_DEBUG_TRACE = "1"
ST_OPTEE_DEBUG_LOG_LEVEL = "1"

# Wic files
WKS_FILE_DEPENDS ?= " \
    virtual/bootloader \
    virtual/trusted-firmware-a \
    ${@bb.utils.contains('BOOTSCHEME_LABELS', 'optee', 'virtual/optee-os', '', d)} \
"

# Disable the generation of flashlayout files
ENABLE_FLASHLAYOUT_CONFIG = "0"

# Remove custom kernel headers from toolchain
ST_TOOLS_FOR_TARGET_SDK:remove = " kernel-headers "
