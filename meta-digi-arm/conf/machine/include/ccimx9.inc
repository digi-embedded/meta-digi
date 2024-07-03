#@TYPE: Machine
#@NAME: ConnectCore 9 family of SOMs
#@DESCRIPTION: Machine configuration for Digi's ConnectCore 9 family of SOMs

DIGI_FAMILY = "ccimx9"

require conf/machine/include/imx-digi-base.inc
require conf/machine/include/arm/armv8-2a/tune-cortexa55.inc

# Platform u-boot settings
UBOOT_DTB_LOADADDRESS = "0x83000000"
UBOOT_DTBO_LOADADDRESS = "0x83000000"
UBOOT_ENTRYPOINT = "0x80400000"
UBOOT_ENV = "boot"
UBOOT_PREFIX = "imx-boot"
UBOOT_SUFFIX = "bin"

# Platform kernel settings (keep the override as otherwise KERNEL_IMAGETYPE
# from imx-digi-base.inc takes precedence)
KERNEL_CLASSES = "kernel-fitimage"
KERNEL_IMAGETYPE:ccimx9 = "${@oe.utils.vartrue('TRUSTFENCE_SIGN_ARTIFACTS', 'fitImage', 'Image.gz', d)}"

# The bootloader image that gets flashed consists of U-Boot and several fw binaries
EXTRA_IMAGEDEPENDS += "imx-boot"
BOOTLOADER_IMAGE_RECIPE = "imx-boot"
BOOTABLE_FILENAME = "${UBOOT_PREFIX}-${MACHINE}.bin"
SDIMG_BOOTLOADER = "${DEPLOY_DIR_IMAGE}/${BOOTABLE_FILENAME}"
BOOTLOADER_SEEK_USERDATA = "32"

# Generate images for all SOC revisions (keep the override as otherwise the
# one in "use-imx-security-controller-firmware.bbclass" takes precedence)
SECO_FIRMWARE_NAME:ccimx9 = "mx93??-ahab-container.img"

DDR_FIRMWARE_NAME = " \
    lpddr4_dmem_1d_v202201.bin \
    lpddr4_dmem_2d_v202201.bin \
    lpddr4_imem_1d_v202201.bin \
    lpddr4_imem_2d_v202201.bin \
"

MACHINE_FEATURES += "pci wifi bluetooth"
MACHINE_FEATURES:append:use-nxp-bsp = " optee jailhouse"

STORAGE_MEDIA = "mmc"
WLAN_P2P_INTERFACE = "wfd0"

# Linux kernel configuration
KERNEL_DEFCONFIG ?= "arch/arm64/configs/ccimx9_defconfig"

SPL_BINARY = "spl/u-boot-spl.bin"

MACHINE_EXTRA_RDEPENDS += " \
    e2fsprogs-mke2fs \
    e2fsprogs-resize2fs \
    parted \
    xbee-init \
"

# TrustFence
TRUSTFENCE_SIGN_MODE = "AHAB"

IMX_BOOT_SEEK = "32"

SERIAL_CONSOLES = "115200;ttyLP5"

# Bluetooth tty
BT_TTY ?= "ttyLP0"

# Per-machine DISTRO_FEATURES customization
MACHINE_DISTRO_FEATURES_REMOVE = "vulkan"

# SWUpdate sw-description configuration
BOOTFS_EXT ?= ".boot.vfat"
ROOTFS_EXT ?= '${@bb.utils.contains("IMAGE_FEATURES", "read-only-rootfs", ".squashfs", ".ext4.gz", d)}'

BOOT_DEV_NAME ?= "/dev/mmcblk0p1"
ROOTFS_DEV_NAME ?= "/dev/mmcblk0p3"
ROOTFS_ENC_DEV = "/dev/mapper/cryptrootfs"
ROOTFS_DEV_NAME_FINAL = "${@oe.utils.ifelse(d.getVar('TRUSTFENCE_ENCRYPT_ROOTFS') == '1', '${ROOTFS_ENC_DEV}', '${ROOTFS_DEV_NAME}')}"

# Wireless external module
WIRELESS_MODULE ?= ""
WIRELESS_MODULE:append = " ${@oe.utils.conditional('HAVE_WIFI', '1', 'kernel-module-nxp-wlan', '', d)}"
HAS_WIFI_VIRTWLANS = "true"

# Machine firmware
MACHINE_FIRMWARE:append = " ${@oe.utils.conditional('HAVE_WIFI', '1', 'firmware-nxp-wifi-nxpiw612 firmware-murata-nxp', '', d)}"

MACHINE_EXTRA_RRECOMMENDS += "${WIRELESS_MODULE}"

# XBee
XBEE_RESET_N_GPIO ?= "gpio2@19"
XBEE_SLEEP_RQ_GPIO ?= ""
XBEE_TTY ?= "ttyLP3"

# Image FS types
IMAGE_FSTYPES ?= '${@bb.utils.contains("IMAGE_FEATURES", "read-only-rootfs", \
				       "boot.vfat recovery.vfat squashfs tar.bz2", \
				       "boot.vfat recovery.vfat ext4.gz tar.bz2 sdcard.gz", d)}'

# Default overlayfs_etc mount point and type
OVERLAYFS_ETC_MOUNT_POINT ?= "/mnt/data"
OVERLAYFS_ETC_DEVICE ?= "/dev/mmcblk0p7"
OVERLAYFS_ETC_FSTYPE ?= "ext4"

PREFERRED_VERSION_gstreamer1.0:ccimx9              ?= "1.22.5.imx"
PREFERRED_VERSION_gstreamer1.0-plugins-bad:ccimx9  ?= "1.22.5.imx"
PREFERRED_VERSION_gstreamer1.0-plugins-base:ccimx9 ?= "1.22.5.imx"
PREFERRED_VERSION_gstreamer1.0-plugins-good:ccimx9 ?= "1.22.5.imx"
PREFERRED_VERSION_gstreamer1.0-plugins-ugly:ccimx9 ?= "1.22.6"

PREFERRED_VERSION_libdrm:ccimx9            ?= "2.4.115.imx"
PREFERRED_VERSION_nativesdk-wayland:ccimx9 ?= "1.22.0.imx"
PREFERRED_VERSION_wayland:ccimx9           ?= "1.22.0.imx"
PREFERRED_VERSION_wayland-native:ccimx9    ?= "1.22.0.imx"
PREFERRED_VERSION_wayland-protocols:ccimx9 ?= "1.32.imx"
PREFERRED_VERSION_weston:ccimx9            ?= "11.0.3.imx"
PREFERRED_VERSION_xwayland:ccimx9          ?= "23.1.1.imx"

PREFERRED_VERSION_nativesdk-trustfence-sign-tools:ccimx9 ?= "2023.04"
PREFERRED_VERSION_trustfence-sign-tools-native:ccimx9    ?= "2023.04"
