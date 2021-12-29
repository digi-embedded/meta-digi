# Digi BSP default providers
PREFERRED_PROVIDER_jpeg-native ?= "jpeg-native"
PREFERRED_PROVIDER_u-boot ??= "u-boot-dey"
PREFERRED_PROVIDER_virtual/bootloader ??= "u-boot-dey"
PREFERRED_PROVIDER_virtual/kernel ??= "linux-dey"
PREFERRED_PROVIDER_virtual/kernel_use-mainline-bsp ??= "linux-fslc"
PREFERRED_PROVIDER_virtual/xserver = "xserver-xorg"

# Update default libc version
LINUXLIBCVERSION ?= "5.4%"

#
# Linux and U-Boot supported versions:
#
# Platform      Linux              U-Boot
# -------------------------------------------------
# ccimx6        5.4               2017.03
# ccimx6qp      5.4               2017.03
# ccimx6ul      5.4               2020.04
# ccimx8x       5.4               2020.04 (packaged in imx-boot)
# ccimx8mn      5.4               2020.04 (packaged in imx-boot)
# ccimx8mm      5.4               2020.04 (packaged in imx-boot)
# ccimx8mp      5.10              2021.04 (packaged in imx-boot)
#

# Help variables used in recipes
HAVE_WIFI       = "${@bb.utils.contains('MACHINE_FEATURES', 'wifi', '1', '', d)}"
HAVE_BT         = "${@bb.utils.contains('MACHINE_FEATURES', 'bluetooth', '1', '', d)}"
HAVE_GUI        = "${@bb.utils.contains('DISTRO_FEATURES', 'x11', '1', '', d)}"

#
# Ethernet, Wi-Fi, and Bluetooth configuration used in recipes
#
# ETHx_MODE, WLANx_MODE accepted values: 'dhcp' or 'static'
#
ETH0_MODE            ?= "dhcp"
ETH0_STATIC_IP       ?= "192.168.42.30"
ETH0_STATIC_NETMASK  ?= "255.255.255.0"
ETH0_STATIC_GATEWAY  ?= "192.168.42.1"
ETH1_MODE            ?= "dhcp"
ETH1_STATIC_IP       ?= "192.168.44.30"
ETH1_STATIC_NETMASK  ?= "255.255.255.0"
WLAN0_MODE           ?= "dhcp"
WLAN0_STATIC_IP      ?= "192.168.43.30"
WLAN0_STATIC_NETMASK ?= "255.255.255.0"
WLAN1_MODE           ?= "static"
WLAN1_STATIC_IP      ?= "192.168.46.30"
WLAN1_STATIC_NETMASK ?= "255.255.255.0"
P2P0_STATIC_IP       ?= "192.168.45.30"
P2P0_STATIC_NETMASK  ?= "255.255.255.0"
WLAN_P2P_INTERFACE   ?= "p2p0"
WLAN_P2P_DEVICE_NAME ??= "${DIGI_FAMILY}-p2p"
BT_DEVICE_NAME       ??= "${DIGI_FAMILY}"

# Digi BSP default settings
IMAGE_CLASSES = "image_types_digi"

# Do not enable console on VT (see sysvinit-inittab recipe)
USE_VT ?= "0"

# Add the machine variant to the valid machine override
MACHINE_VARIANT ?= ""
MACHINEOVERRIDES .= "${@['', ':${MACHINE_VARIANT}']['${MACHINE_VARIANT}' != '']}"

# U-Boot symlink
UBOOT_SYMLINK ?= "${UBOOT_PREFIX}-${MACHINE}.${UBOOT_SUFFIX}"
BOOTABLE_ARTIFACTS ?= ""

# Extra RDEPENDS
MACHINE_EXTRA_RDEPENDS += " \
    ${@bb.utils.contains("MACHINE_FEATURES", "mca", "mca-tool", "", d)} \
"
# Extra RRECOMMENDS
MACHINE_EXTRA_RRECOMMENDS += " \
    ${@bb.utils.contains("MACHINE_FEATURES", "cryptochip", "cryptoauthlib", "", d)} \
"

# Default image name (for install scripts)
DEFAULT_IMAGE_NAME ??= "dey-image-qt"

# List of graphical images names (for install scripts)
GRAPHICAL_IMAGES ?= "dey-image-qt dey-image-webkit"

# Include DEY SELinux policy modifications by default
DEY_SELINUX_POLICY ?= "1"

# U-Boot scripts to include in 'linux' partition
# (use the '+=' operator, since other layers may append scripts to this list)
BOOT_SCRIPTS += "boot.scr:boot.scr"
