#@TYPE: Machine
#@NAME: ConnectCore 6 module.
#@DESCRIPTION: Machine configuration for Digi's ConnectCore 6 module.

include conf/machine/include/imx-digi-base.inc
include conf/machine/include/tune-cortexa9.inc

DIGI_FAMILY = "ccimx6"
SOC_FAMILY  = "mx6:mx6dl:mx6q:${DIGI_FAMILY}"

# Platform u-boot settings
UBOOT_ENTRYPOINT = "0x10800000"
UBOOT_SUFFIX = "imx"
UBOOT_SYMLINK = "u-boot-${MACHINE}.${UBOOT_SUFFIX}"

# wpa_supplicant driver to use in '/etc/network/interfaces'
WPA_DRIVER = "nl80211"

# Wireless external module
WIRELESS_MODULE ?= ""

# Firmware
MACHINE_FIRMWARE ?= ""
MACHINE_FIRMWARE_append_mx6q = " firmware-imx-vpu-imx6q"
MACHINE_FIRMWARE_append_mx6dl = " firmware-imx-vpu-imx6d"
MACHINE_FIRMWARE_append = " ${@base_conditional('HAVE_BT', '1' , 'firmware-atheros-ar3k', '', d)}"

MACHINE_EXTRA_RDEPENDS += " \
    e2fsprogs-mke2fs \
    e2fsprogs-resize2fs \
    parted \
    u-boot-fw-utils \
"
MACHINE_EXTRA_RRECOMMENDS += "${MACHINE_FIRMWARE} ${WIRELESS_MODULE}"
MACHINE_EXTRA_RRECOMMENDS += "fsl-alsa-plugins"

MACHINE_FEATURES += "accel-graphics accel-video wifi bluetooth"
