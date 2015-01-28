#@TYPE: Machine
#@NAME: ConnectCore for MX28 module.
#@DESCRIPTION: Machine configuration for Digi's ConnectCore for MX28 module.

include conf/machine/include/mxs-digi-base.inc

DIGI_FAMILY = "ccardimx28"
SOC_FAMILY  = "mxs:mx28:${DIGI_FAMILY}"

# Disable perf features when using kernel 2.6.35 as perf fails to build otherwise
python __anonymous() {
    if d.getVar('IS_KERNEL_2X', True) == '1':
        d.setVar('PERF_FEATURES_ENABLE', '')
}

# Platform u-boot settings (IS_KERNEL_2X implies also u-boot v2009.08)
UBOOT_BSTR = "${@base_conditional('IS_KERNEL_2X', '1' , '-ivt', '', d)}"
UBOOT_ENTRYPOINT = "0x40008000"
UBOOT_SUFFIX = "sb"

# wpa_supplicant driver to use in '/etc/network/interfaces'
WPA_DRIVER = "nl80211"

# Firmware
MACHINE_FIRMWARE ?= ""
MACHINE_FIRMWARE_append = " ${@base_conditional('HAVE_BT', '1' , 'firmware-atheros-ar3k', '', d)}"

MACHINE_EXTRA_RDEPENDS    += "mtd-utils-ubifs nvram ubootenv update-flash"
MACHINE_EXTRA_RRECOMMENDS += "${MACHINE_FIRMWARE}"

MACHINE_FEATURES += "alsa touchscreen"

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

# UBOOT_CONFIG_VARIANT for different CPU speed (default is empty)
#UBOOT_CONFIG_VARIANT ?= "261MHz"
#UBOOT_CONFIG_VARIANT ?= "360MHz"
