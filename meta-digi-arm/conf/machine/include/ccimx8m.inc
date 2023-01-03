#@TYPE: Machine
#@NAME: ConnectCore 8M family of SOMs.
#@DESCRIPTION: Machine configuration for Digi's ConnectCore 8M family of SOMs.

include conf/machine/include/imx-digi-base.inc
require conf/machine/include/arm/armv8a/tune-cortexa53.inc

# Platform u-boot settings
UBOOT_PREFIX = "imx-boot"
UBOOT_SUFFIX = "bin"

# The bootloader image that gets flashed consists of U-Boot and several fw binaries
EXTRA_IMAGEDEPENDS += "imx-boot"
BOOTLOADER_IMAGE_RECIPE = "imx-boot"
BOOTABLE_FILENAME = "${UBOOT_PREFIX}-${MACHINE}.bin"
SDIMG_BOOTLOADER = "${DEPLOY_DIR_IMAGE}/${BOOTABLE_FILENAME}"

# LPDDR4 firmware
DDR_FIRMWARE_NAME = " \
    lpddr4_pmu_train_1d_imem.bin \
    lpddr4_pmu_train_1d_dmem.bin \
    lpddr4_pmu_train_2d_imem.bin \
    lpddr4_pmu_train_2d_dmem.bin \
"
IMXBOOT_TARGETS = "flash_evk"

# Linux kernel configuration
KERNEL_DEFCONFIG ?= "arch/arm64/configs/ccimx8_defconfig"

STORAGE_MEDIA = "mmc"

# Wireless external module
WIRELESS_MODULE ?= ""
WIRELESS_MODULE:append = " ${@oe.utils.conditional('HAVE_WIFI', '1', 'kernel-module-qualcomm', '', d)}"
HAS_WIFI_VIRTWLANS = "true"

# Firmware
MACHINE_FIRMWARE ?= "firmware-imx-sdma-imx7d firmware-imx-easrc-imx8mn"
MACHINE_FIRMWARE:append = " ${@oe.utils.conditional('HAVE_BT', '1', 'firmware-qualcomm-qca6564-bt', '', d)}"
MACHINE_FIRMWARE:append = " ${@oe.utils.conditional('HAVE_WIFI', '1', 'firmware-qualcomm-qca6564-wifi', '', d)}"

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

MACHINE_FEATURES += "accel-graphics accel-video wifi bluetooth cryptochip mca optee"

# AARCH64 doesn't support self-extracting zImage
KERNEL_IMAGETYPE = "Image.gz"

VIRTUAL-RUNTIME_init_manager ?= "systemd"
VIRTUAL-RUNTIME_initscripts ?= "initscripts"

# TrustFence
TRUSTFENCE_SIGN_MODE = "HAB"

# Adding 'wayland' along with 'x11' enables the xwayland backend
# Vulkan is necessary for wayland to build
DISTRO_FEATURES:append = " wayland vulkan systemd pam"
DISTRO_FEATURES_BACKFILL_CONSIDERED = "sysvinit"

# SWUpdate sw-description configuration
BOOTFS_EXT ?= ".boot.vfat"
ROOTFS_EXT ?= ".ext4.gz"

BOOT_DEV_NAME ?= "/dev/mmcblk0p1"
ROOTFS_DEV_NAME ?= "/dev/mmcblk0p3"
ROOTFS_ENC_DEV = "/dev/mapper/cryptrootfs"
ROOTFS_DEV_NAME_FINAL = "${@oe.utils.ifelse(d.getVar('TRUSTFENCE_ENCRYPT_ROOTFS', True) == '1', '${ROOTFS_ENC_DEV}', '${ROOTFS_DEV_NAME}')}"

UBOOT_DEV_NAME ?= "/dev/mmcblk0boot0"
