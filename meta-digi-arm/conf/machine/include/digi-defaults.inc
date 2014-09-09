# Digi BSP default providers
PREFERRED_PROVIDER_jpeg-native ?= "jpeg-native"
PREFERRED_PROVIDER_u-boot ?= "u-boot-dey"
PREFERRED_PROVIDER_virtual/kernel ?= "linux-dey"
PREFERRED_PROVIDER_virtual/libx11 ?= "${@base_contains('DISTRO_FEATURES', 'x11', 'libx11', '', d)}"
PREFERRED_PROVIDER_virtual/xserver = "xserver-xorg"

#
# Linux and U-Boot supported versions:
#
# Platform      Linux              U-Boot
# -------------------------------------------------
# ccimx5        2.6.35.14          2009.08
# ccardimx28    2.6.35.14, 3.10    2009.08, 2013.01
# ccimx6        3.10               2013.04
#
# Digi BSP default versions
PREFERRED_VERSION_linux-dey ?= "3.10"
PREFERRED_VERSION_linux-dey_ccimx5 ?= "2.6.35.14"
PREFERRED_VERSION_u-boot-dey ?= "2013.01"
PREFERRED_VERSION_u-boot-dey_ccimx5 ?= "2009.08"
PREFERRED_VERSION_u-boot-dey_ccimx6 ?= "2013.04"

# Help variables used in recipes
HAVE_WIFI    = "${@base_contains('MACHINE_FEATURES', 'wifi', '1', '', d)}"
HAVE_EXT_ETH = "${@base_contains('MACHINE_FEATURES', 'ext-eth', '1', '', d)}"
HAVE_BT      = "${@base_contains('MACHINE_FEATURES', 'bluetooth', '1', '', d)}"
HAVE_1WIRE   = "${@base_contains('MACHINE_FEATURES', '1-wire', '1', '', d)}"
HAVE_GUI     = "${@base_contains('DISTRO_FEATURES', 'x11', '1', '', d)}"
HAVE_EXAMPLE = "${@base_contains('IMAGE_FEATURES', 'dey-examples', '1', '', d)}"
IS_KERNEL_2X = "${@base_version_less_or_equal('PREFERRED_VERSION_linux-dey', '2.6.35.14', '1', '', d)}"

#
# Ethernet configuration used in recipes
#
# ETHx_MODE, WLANx_MODE accepted values: 'dhcp' or 'static'
#
ETH0_MODE            ?= "static"
ETH0_STATIC_IP       ?= "192.168.42.30"
ETH0_STATIC_NETMASK  ?= "255.255.255.0"
ETH0_STATIC_GATEWAY  ?= "192.168.42.1"
ETH1_MODE            ?= "static"
ETH1_STATIC_IP       ?= "192.168.44.30"
ETH1_STATIC_NETMASK  ?= "255.255.255.0"
WLAN0_MODE           ?= "static"
WLAN0_STATIC_IP      ?= "192.168.43.30"
WLAN0_STATIC_NETMASK ?= "255.255.255.0"

# Digi BSP default settings
IMAGE_CLASSES = "image_types_digi"

# Do not enable console on VT (see sysvinit-inittab recipe)
USE_VT = "0"

# Add the machine variant to the valid machine override
MACHINEOVERRIDES .= "${@['', ':${MACHINE_VARIANT}']['${MACHINE_VARIANT}' != '']}"
MACHINE_VARIANT ?= ""

# U-Boot settings
UBOOT_BSTR           ??= ""
UBOOT_CONFIG_VARIANT ??= ""
UBOOT_BINARY          ?= "u-boot${UBOOT_BSTR}.${UBOOT_SUFFIX}"
UBOOT_IMAGE           ?= 'u-boot-${MACHINE}${@base_conditional( "UBOOT_CONFIG_VARIANT", "", "", "_${UBOOT_CONFIG_VARIANT}", d )}${UBOOT_BSTR}-${PV}-${PR}.${UBOOT_SUFFIX}'
UBOOT_MACHINE         ?= '${MACHINE}${@base_conditional( "UBOOT_CONFIG_VARIANT", "", "", "_${UBOOT_CONFIG_VARIANT}", d )}_config'
UBOOT_MAKE_TARGET     ?= "${UBOOT_BINARY}"
UBOOT_SYMLINK         ?= 'u-boot-${MACHINE}${@base_conditional( "UBOOT_CONFIG_VARIANT", "", "", "_${UBOOT_CONFIG_VARIANT}", d )}${UBOOT_BSTR}.${UBOOT_SUFFIX}'
