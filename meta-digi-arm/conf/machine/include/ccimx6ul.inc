#@TYPE: Machine
#@NAME: ConnectCore for i.MX6UL.
#@DESCRIPTION: Machine configuration for Digi's ConnectCore 6UL.

DIGI_FAMILY = "ccimx6ul"
MACHINEOVERRIDES =. "mx6:mx6ul:${DIGI_FAMILY}:"

include conf/machine/include/imx-digi-base.inc
include conf/machine/include/tune-cortexa7.inc

# Platform u-boot settings
UBOOT_PREFIX = "u-boot"
UBOOT_SUFFIX = "imx"
UBOOT_HAS_FASTBOOT = "true"

STORAGE_MEDIA = "mtd"

# Wireless external module
WIRELESS_MODULE ?= ""
WIRELESS_MODULE:append = " ${@oe.utils.conditional('HAVE_WIFI', '1', 'kernel-module-qualcomm', '', d)}"
HAS_WIFI_VIRTWLANS = "true"

# Firmware
MACHINE_FIRMWARE ?= "firmware-imx-sdma-imx6q"
MACHINE_FIRMWARE:append = " ${@oe.utils.conditional('HAVE_BT', '1', 'firmware-qualcomm-qca6564-bt', '', d)}"
MACHINE_FIRMWARE:append = " ${@oe.utils.conditional('HAVE_WIFI', '1', 'firmware-qualcomm-qca6564-wifi', '', d)}"

MACHINE_EXTRA_RDEPENDS += " \
    imx-kobs \
    mtd-utils-ubifs \
"

MACHINE_EXTRA_RRECOMMENDS += " \
    ${MACHINE_FIRMWARE} \
    ${WIRELESS_MODULE} \
    ${@bb.utils.contains('COMBINED_FEATURES', 'alsa', 'imx-alsa-plugins', '', d)} \
"
MACHINE_EXTRA_RRECOMMENDS:remove:use-mainline-bsp = "${@bb.utils.contains('COMBINED_FEATURES', 'alsa', 'imx-alsa-plugins', '', d)}"

MACHINE_FEATURES += "wifi bluetooth cryptochip mca"
MACHINE_FEATURES:remove:use-mainline-bsp = "wifi"

# mkfs.ubifs parameters for boot partition (the one holding kernel and device tree files)
# Max LEB count (-c 255) calculated for a partition of up to 32 MiB considering 128 KiB erase-block size.
MKUBIFS_BOOT_ARGS ?= "-m 2048 -e 126976 -c 255"

# mkfs.ubifs parameters for rootfs partition
# Max LEB count (-c 8191) calculated for a partition of up to 1 GiB considering 128 KiB erase-block size.
MKUBIFS_ARGS ?= "-m 2048 -e 126976 -c 8191"

# TrustFence
TRUSTFENCE_SIGN_MODE = "HAB"
