#@TYPE: Machine
#@NAME: ConnectCore for i.MX6UL.
#@DESCRIPTION: Machine configuration for Digi's ConnectCore 6UL.

DIGI_SOM = "ccimx6ul"
DIGI_FAMILY = "ccimx6ul"
MACHINEOVERRIDES =. "mx6:mx6ul:${DIGI_FAMILY}:${DIGI_SOM}:"

include conf/machine/include/imx-digi-base.inc
include conf/machine/include/arm/armv7a/tune-cortexa7.inc

# Override default init manager from distro config
INIT_MANAGER ?= "sysvinit"

# Platform u-boot settings
UBOOT_PREFIX = "u-boot"
UBOOT_SUFFIX = "imx"

STORAGE_MEDIA = "mtd"

# Wireless external module
WIRELESS_MODULE ?= ""
WIRELESS_MODULE:append = " ${@oe.utils.conditional('HAVE_WIFI', '1', 'kernel-module-qualcomm', '', d)}"
HAS_WIFI_VIRTWLANS = "true"

# Machine firmware
MACHINE_FIRMWARE:append = " ${@oe.utils.conditional('HAVE_BT', '1', 'firmware-qualcomm-qca6564-bt', '', d)}"
MACHINE_FIRMWARE:append = " ${@oe.utils.conditional('HAVE_WIFI', '1', 'firmware-qualcomm-qca6564-wifi', '', d)}"

MACHINE_EXTRA_RDEPENDS += " \
    imx-kobs \
    mtd-utils-ubifs \
"

MACHINE_EXTRA_RRECOMMENDS += "${WIRELESS_MODULE}"

MACHINE_FEATURES += "wifi bluetooth cryptochip mca"
MACHINE_FEATURES:remove:use-mainline-bsp = "wifi"

# mkfs.ubifs parameters for boot partition (the one holding kernel and device tree files)
# Max LEB count (-c 255) calculated for a partition of up to 32 MiB considering 128 KiB erase-block size.
MKUBIFS_BOOT_ARGS ?= "-m 2048 -e 126976 -c 255"

# mkfs.ubifs parameters for recovery partition
# Same parameters as the boot partition, but using zlib compression to reduce image size.
MKUBIFS_RECOVERY_ARGS ?= "${MKUBIFS_BOOT_ARGS} -x zlib"

# mkfs.ubifs parameters for rootfs partition
# Max LEB count (-c 8191) calculated for a partition of up to 1 GiB considering 128 KiB erase-block size.
MKUBIFS_ARGS ?= "-m 2048 -e 126976 -c 8191"

# TrustFence
TRUSTFENCE_SIGN_MODE = "HAB"

# Per-machine DISTRO_FEATURES customization
MACHINE_DISTRO_FEATURES_REMOVE = "vulkan wayland"

# SWUpdate sw-description configuration
BOOTFS_EXT ?= ".boot.ubifs"
ROOTFS_EXT ?= '${@bb.utils.contains("IMAGE_FEATURES", "read-only-rootfs", ".squashfs", ".ubifs", d)}'

BOOT_DEV_NAME ?= "linux"
ROOTFS_DEV_NAME ?= "rootfs"
ROOTFS_ENC_DEV = "${ROOTFS_DEV_NAME}"
ROOTFS_DEV_NAME_FINAL = "${@oe.utils.ifelse(d.getVar('TRUSTFENCE_ENCRYPT_ROOTFS') == '1', '${ROOTFS_ENC_DEV}', '${ROOTFS_DEV_NAME}')}"

# U-Boot environment offset (within partition)
UBOOT_ENV_OFFSET ?= "0x0"
UBOOT_ENV_REDUND_OFFSET ?= "${UBOOT_ENV_SIZE}"
# U-Boot environment size
UBOOT_ENV_SIZE ?= "0x20000"
# U-Boot environment range: size (in hex) in the environment partition that
# the U-Boot environment can take up (if undefined, it will take up all the
# available space in the environment partition)
UBOOT_ENV_RANGE ?= ""

# Partitions to blacklist for swupdate:
# bootloader, environment, safe
SWUPDATE_MTD_BLACKLIST = "0 1 2"
