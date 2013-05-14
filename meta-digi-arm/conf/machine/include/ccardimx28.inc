#@TYPE: Machine
#@NAME: ConnectCore for MX28 module.
#@DESCRIPTION: Machine configuration for Digi's ConnectCore for MX28 module.

include conf/machine/include/mxs-digi-base.inc

SOC_FAMILY = "mxs"

UBOOT_ENTRYPOINT  = "0x41008000"
UBOOT_LOADADDRESS = "0x41008000"

# Set UBOOT_CONFIG_VARIANT in local.conf accordingly, default is empty: 261MHz,360MHz,test,dbg
UBOOT_CONFIG_VARIANT ?= ""
UBOOT_MACHINE = '${MACHINE}${@base_conditional( "UBOOT_CONFIG_VARIANT", "", "_", "_${UBOOT_CONFIG_VARIANT}_", d )}config'
UBOOT_MAKE_TARGET = "u-boot-ivt.sb"
UBOOT_SUFFIX = "sb"
UBOOT_IMAGE = 'u-boot-${MACHINE}${@base_conditional( "UBOOT_CONFIG_VARIANT", "", "-", "-${UBOOT_CONFIG_VARIANT}-", d )}ivt-${PV}-${PR}.${UBOOT_SUFFIX}'
UBOOT_BINARY = "u-boot-ivt.${UBOOT_SUFFIX}"
UBOOT_SYMLINK = 'u-boot-${MACHINE}${@base_conditional( "UBOOT_CONFIG_VARIANT", "", "-", "-${UBOOT_CONFIG_VARIANT}-", d )}ivt.${UBOOT_SUFFIX}'

# wpa_supplicant driver to use in '/etc/network/interfaces'
WPA_DRIVER = "nl80211"

# Firmware
MACHINE_FIRMWARE = "firmware-mxs-ar3k"
MACHINE_EXTRA_RRECOMMENDS += "${MACHINE_FIRMWARE}"

MACHINE_FEATURES += "alsa touchscreen"

#
# Supported variants
#
# To build any of these variants configure MACHINE_VARIANT on the project's
# local.conf to the variant name.
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

# Variants for different CPU speed
#UBOOT_CONFIG_VARIANT = "261MHz"
#UBOOT_CONFIG_VARIANT = "360MHz"
