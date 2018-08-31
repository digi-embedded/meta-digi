#@TYPE: Machine
#@NAME: ConnectCore for i.MX6UL.
#@DESCRIPTION: Machine configuration for Digi's ConnectCore 6UL.

DIGI_FAMILY = "ccimx6ul"
MACHINEOVERRIDES =. "mx6:mx6ul:${DIGI_FAMILY}:"

include conf/machine/include/imx-digi-base.inc
include conf/machine/include/tune-cortexa7.inc

# Platform u-boot settings
UBOOT_SUFFIX = "imx"
UBOOT_SYMLINK = "u-boot-${MACHINE}.${UBOOT_SUFFIX}"

# Linux kernel configuration
KERNEL_DEFCONFIG ?= "arch/arm/configs/ccimx6ul_defconfig"

# Wireless external module
WIRELESS_MODULE ?= ""
WIRELESS_MODULE_append = " ${@base_conditional('HAVE_WIFI', '1', 'kernel-module-qualcomm', '', d)}"

# Wireless p2p interface
WLAN_P2P_INTERFACE ?= "p2p0"

# Firmware
MACHINE_FIRMWARE ?= ""

MACHINE_EXTRA_RDEPENDS += " \
    imx-kobs \
    mca-tool \
    mtd-utils-ubifs \
    u-boot-fw-utils \
"

MACHINE_EXTRA_RRECOMMENDS += " \
    ${MACHINE_FIRMWARE} \
    ${WIRELESS_MODULE} \
    ${@bb.utils.contains('COMBINED_FEATURES', 'alsa', 'imx-alsa-plugins', '', d)} \
    cryptoauthlib \
    cryptodev-module \
    cryptoauth-openssl-engine \
"

MACHINE_FEATURES += "wifi bluetooth cryptochip"

MACHINE_FIRMWARE_append = " ${@base_conditional('HAVE_BT', '1', 'firmware-qualcomm-qca6564-bt', '', d)}"
MACHINE_FIRMWARE_append = " ${@base_conditional('HAVE_WIFI', '1', 'firmware-qualcomm-qca6564-wifi', '', d)}"

# mkfs.ubifs parameters for boot partition (the one holding kernel and device tree files)
# Max LEB count (-c 255) calculated for a partition of up to 32 MiB considering 128 KiB erase-block size.
MKUBIFS_BOOT_ARGS ?= "-m 2048 -e 126976 -c 255"

# mkfs.ubifs parameters for rootfs partition
# Max LEB count (-c 8191) calculated for a partition of up to 1 GiB considering 128 KiB erase-block size.
MKUBIFS_ARGS ?= "-m 2048 -e 126976 -c 8191"
