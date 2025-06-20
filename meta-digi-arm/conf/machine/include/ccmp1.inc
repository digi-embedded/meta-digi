#@TYPE: Machine
#@NAME: ConnectCore for STM32MP1.
#@DESCRIPTION: Machine configuration for Digi's ConnectCore MP1.

include conf/machine/include/st-machine-common-stm32mp.inc
include conf/machine/include/arm/armv7a/tune-cortexa7.inc
include conf/machine/include/digi-defaults.inc

DIGI_FAMILY = "ccmp1"

# Platform T-FA settings
TFA_SIGN_SUFFIX ?= "${@bb.utils.contains('TRUSTFENCE_SIGN', '1', '_signed', '', d)}"

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
UBOOT_ENTRYPOINT = "0xc2000000"
UBOOT_DTB_LOADADDRESS = "0xc4000000"
UBOOT_DTBO_LOADADDRESS = "0xc4000000"
# Set InitRAMDisk load address to U-boot initrd_addr
UBOOT_RD_LOADADDRESS = "0xc4400000"
# U-Boot environment offset (within partition)
UBOOT_ENV_OFFSET ?= "0x0"
UBOOT_ENV_REDUND_OFFSET ?= "0x0"
# U-Boot environment size
UBOOT_ENV_SIZE ?= "0x20000"
# U-Boot environment range: size (in hex) in the environment partition that
# the U-Boot environment can take up (if undefined, it will take up all the
# available space in the environment partition)
UBOOT_ENV_RANGE ?= ""

FIP_SIGN_SUFFIX ?= "${@bb.utils.contains('TRUSTFENCE_SIGN', '1', '_Signed', '', d)}"

# =========================================================================
# Kernel
# =========================================================================
KERNEL_IMAGETYPE = "${@bb.utils.contains('TRUSTFENCE_SIGN_FIT_STM', '1', 'fitImage', 'zImage', d)}"
KERNEL_CLASSES = "kernel-fitimage"
KERNEL_ALT_IMAGETYPE = "Image vmlinux"
KERNEL_DEFCONFIG ?= "arch/arm/configs/ccmp1_defconfig"

STORAGE_MEDIA = "mtd"

# Enable STM specific features
DEY_SOC_VENDOR = "STM"

# SWUpdate sw-description configuration
BOOTFS_EXT ?= ".boot.ubifs"
ROOTFS_EXT ?= '${@bb.utils.contains("IMAGE_FEATURES", "read-only-rootfs", ".squashfs", ".ubifs", d)}'

BOOT_DEV_NAME ?= "linux"
ROOTFS_DEV_NAME ?= "rootfs"
ROOTFS_ENC_DEV = "${ROOTFS_DEV_NAME}"
ROOTFS_DEV_NAME_FINAL = "${@oe.utils.ifelse(d.getVar('TRUSTFENCE_ENCRYPT_ROOTFS') == '1', '${ROOTFS_ENC_DEV}', '${ROOTFS_DEV_NAME}')}"

# Disable by default programmer support:
STM32MP_DEVICETREE_PROGRAMMER_ENABLE ?= "0"

# OPTEE runtime packages to install
OPTEE_PKGS ??= "optee-client"

MACHINE_EXTRA_RDEPENDS += " \
    mtd-utils-ubifs \
"
MACHINE_EXTRA_RRECOMMENDS:remove = "kernel-imagebootfs"

# =========================================================================
# alsa
# =========================================================================
ALSA_LIST:stm32mp1common = ""

# Image FS types (set by meta-st-stm32mp layer)
# tar.xz is omitted here because it's already added by ST layer
IMAGE_FSTYPES += '${@bb.utils.contains("IMAGE_FEATURES", "read-only-rootfs", \
                                       "boot.ubifs recovery.ubifs squashfs", \
                                       "boot.ubifs recovery.ubifs ubifs", d)}'
IMAGE_FSTYPES:remove = "ext4"
IMAGE_FSTYPES:append = " ${@bb.utils.contains('BOOTDEVICE_LABELS', 'sdcard', 'boot.vfat ext4.gz', '', d)}"

# List of supported boot devices
BOOTDEVICE_LABELS ?= "nand-4-256-1024 usb"

# Remove NAND-based ubi/ubifs images added by st-machine-common-stm32mp.inc
# because image_types_digi.bbclass already builds their own.
MULTIUBI_BUILD:remove = "nand_4_256_1024"
IMAGE_FSTYPES:remove = "multiubi"

# Wic files
WKS_FILE_DEPENDS ?= " \
    virtual/bootloader \
    virtual/trusted-firmware-a \
    ${@bb.utils.contains('BOOTSCHEME_LABELS', 'optee', 'virtual/optee-os', '', d)} \
"

# Disable the generation of flashlayout files
ENABLE_FLASHLAYOUT_CONFIG = "0"

# Partitions to blacklist for swupdate:
# fsbl1, fsbl2, metadata1, metadata2, fip-a, fip-b
SWUPDATE_MTD_BLACKLIST = "0 1 2 3 4 5"

# Remove custom kernel headers from toolchain
ST_TOOLS_FOR_TARGET_SDK:remove = "kernel-headers"

# Skip warnings for packages using 32-bit APIs
INSANE_SKIP:append:pn-alsa-lib = " 32bit-time"
INSANE_SKIP:append:pn-ir-keytable = " 32bit-time"
INSANE_SKIP:append:pn-kbd = " 32bit-time"
INSANE_SKIP:append:pn-v4l-utils = " 32bit-time"
