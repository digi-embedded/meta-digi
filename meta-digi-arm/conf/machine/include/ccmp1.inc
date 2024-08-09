#@TYPE: Machine
#@NAME: ConnectCore for STM32MP1.
#@DESCRIPTION: Machine configuration for Digi's ConnectCore MP1.

include conf/machine/include/st-machine-common-stm32mp.inc
include conf/machine/include/arm/armv7a/tune-cortexa7.inc
include conf/machine/include/digi-defaults.inc

DIGI_FAMILY = "ccmp1"

# Platform T-FA settings
TFA_SIGN_SUFFIX ?= "${@bb.utils.contains('TRUSTFENCE_SIGN', '1', '_signed', '', d)}"

# Platform u-boot settings
UBOOT_PREFIX = "fip"
UBOOT_SUFFIX = "bin"
# Set U-Boot FIP settings
FIP_UBOOT_DTB = "u-boot"
FIP_SIGN_SUFFIX ?= "${@bb.utils.contains('TRUSTFENCE_SIGN', '1', '_Signed', '', d)}"

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

# U-Boot environment offset (within partition)
UBOOT_ENV_OFFSET ?= "0x0"
UBOOT_ENV_REDUND_OFFSET ?= "0x0"
# U-Boot environment size
UBOOT_ENV_SIZE ?= "0x20000"
# U-Boot environment range: size (in hex) in the environment partition that
# the U-Boot environment can take up (if undefined, it will take up all the
# available space in the environment partition)
UBOOT_ENV_RANGE ?= ""

# Set InitRAMDisk load address to U-boot initrd_addr
UBOOT_RD_LOADADDRESS = "0xc4400000"

# OPTEE runtime packages to install
OPTEE_PKGS ??= "optee-client"

MACHINE_EXTRA_RDEPENDS += " \
    mtd-utils-ubifs \
"
MACHINE_EXTRA_RRECOMMENDS:remove = "kernel-imagebootfs"

# Image FS types
IMAGE_FSTYPES:remove = "ext4 tar.xz"
IMAGE_FSTYPES:append = " ${@bb.utils.contains('BOOTDEVICE_LABELS', 'sdcard', 'boot.vfat ext4.gz', '', d)}"

# List of supported boot devices
BOOTDEVICE_LABELS ?= "nand-4-256"
# Supported boot schemes
BOOTSCHEME_LABELS ?= "optee"

# Remove NAND-based ubi/ubifs images added by st-machine-common-stm32mp.inc
# because image_types_digi.bbclass already builds their own.
MULTIUBI_BUILD:remove = "nand_4_256"
IMAGE_FSTYPES:remove = "stmultiubi"

# Wic files
WKS_FILE_DEPENDS ?= " \
    virtual/bootloader \
    virtual/trusted-firmware-a \
    ${@bb.utils.contains('BOOTSCHEME_LABELS', 'optee', 'virtual/optee-os', '', d)} \
"

# Default secure console configuration
TRUSTFENCE_CONSOLE_DISABLE ?= "0"

# Alternatively, uncommment to enable the console autoboot stop passphrase
# TRUSTFENCE_CONSOLE_PASSPHRASE_ENABLE = ""

# Disable the generation of flashlayout files
do_create_flashlayout_config[noexec] = "1"

# Include boot script into the FIT image
UBOOT_ENV = "boot"

# Partitions to blacklist for swupdate:
# fsbl1, fsbl2, metadata1, metadata2, fip-a, fip-b
SWUPDATE_MTD_BLACKLIST = "0 1 2 3 4 5"
