#@TYPE: Machine
#@NAME: ConnectCore 6 module.
#@DESCRIPTION: Machine configuration for Digi's ConnectCore 6 module.

DIGI_FAMILY = "ccimx6"
MACHINEOVERRIDES =. "mx6:mx6dl:mx6q:${DIGI_FAMILY}:"

include conf/machine/include/imx-digi-base.inc
include conf/machine/include/tune-cortexa9.inc

# Platform u-boot settings
UBOOT_ENTRYPOINT = "0x10800000"
UBOOT_SUFFIX = "imx"
UBOOT_SYMLINK = "u-boot-${MACHINE}.${UBOOT_SUFFIX}"

# Wireless external module
WIRELESS_MODULE ?= ""

# Firmware
MACHINE_FIRMWARE ?= ""
MACHINE_FIRMWARE_append_mx6q = " firmware-imx-vpu-imx6q"
MACHINE_FIRMWARE_append_mx6dl = " firmware-imx-vpu-imx6d"
MACHINE_FIRMWARE_append = " ${@base_conditional('HAVE_BT', '1' , 'firmware-atheros-ar3k', '', d)}"
MACHINE_FIRMWARE_append = " ${@base_conditional('HAVE_WIFI', '1' , 'firmware-atheros-ath6kl', '', d)}"

MACHINE_EXTRA_RDEPENDS += " \
    e2fsprogs-mke2fs \
    e2fsprogs-resize2fs \
    parted \
    u-boot-fw-utils \
"
MACHINE_EXTRA_RRECOMMENDS += "${MACHINE_FIRMWARE} ${WIRELESS_MODULE}"
MACHINE_EXTRA_RRECOMMENDS += "imx-alsa-plugins cryptodev-module rng-tools"

MACHINE_FEATURES += "accel-graphics accel-video wifi bluetooth"
