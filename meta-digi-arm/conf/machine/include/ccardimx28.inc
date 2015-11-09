#@TYPE: Machine
#@NAME: ConnectCore for MX28 module.
#@DESCRIPTION: Machine configuration for Digi's ConnectCore for MX28 module.

include conf/machine/include/imx-digi-base.inc
include conf/machine/include/tune-arm926ejs.inc

DIGI_FAMILY = "ccardimx28"
SOC_FAMILY  = "mxs:mx28:${DIGI_FAMILY}"

# Platform u-boot settings
UBOOT_ENTRYPOINT = "0x40008000"
UBOOT_SUFFIX = "sb"
UBOOT_SYMLINK = "u-boot-${MACHINE}.${UBOOT_SUFFIX}"

# Wireless external module
WIRELESS_MODULE ?= ""
WIRELESS_MODULE_append = "${@base_contains('MACHINE_FEATURES', 'wifi', 'kernel-module-atheros', '', d)}"

# Firmware
MACHINE_FIRMWARE ?= ""
MACHINE_FIRMWARE_append = " ${@base_conditional('HAVE_BT', '1' , 'firmware-atheros-ar3k', '', d)}"
MACHINE_FIRMWARE_append = " ${@base_conditional('HAVE_WIFI', '1' , 'firmware-atheros-ath6kl', '', d)}"

MACHINE_EXTRA_RDEPENDS    += "mtd-utils-ubifs nvram ubootenv update-flash"
MACHINE_EXTRA_RRECOMMENDS += "${MACHINE_FIRMWARE} ${WIRELESS_MODULE}"

#
# Supported variants
#
# Maintain in sync with the same table in platform local.conf template.
#
# Name          WiFi    Eth2    BT      1wire
# -------------------------------------------
# -             N       N       N       N       (empty MACHINE_VARIANT="")
# e             N       Y       N       N
# w             Y       N       N       N
# wb            Y       N       Y       N
# web           Y       Y       Y       N
# web1          Y       Y       Y       Y

# Per-variant machine features
MACHINE_FEATURES_append_e = " ext-eth"
MACHINE_FEATURES_append_w = " wifi"
MACHINE_FEATURES_append_wb = " wifi bluetooth"
MACHINE_FEATURES_append_web = " wifi ext-eth bluetooth"
MACHINE_FEATURES_append_web1 = " wifi ext-eth bluetooth 1-wire"
