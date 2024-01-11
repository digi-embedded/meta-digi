# Digi BSP default providers
PREFERRED_PROVIDER_u-boot ??= "u-boot-dey"
PREFERRED_PROVIDER_virtual/bootloader ??= "u-boot-dey"
PREFERRED_PROVIDER_virtual/kernel ??= "linux-dey"
PREFERRED_PROVIDER_virtual/kernel:use-mainline-bsp ??= "linux-fslc"
PREFERRED_PROVIDER_virtual/xserver = "xserver-xorg"

#
# Linux and U-Boot supported versions:
#
# Platform      Linux              U-Boot
# -------------------------------------------------
# ccimx6        5.4               2017.03
# ccimx6qp      5.4               2017.03
# ccimx6ul      5.15              2020.04
# ccimx8x       5.4               2020.04 (packaged in imx-boot)
# ccimx8mn      5.4               2020.04 (packaged in imx-boot)
# ccimx8mm      5.4               2020.04 (packaged in imx-boot)
# ccimx93       5.15              2022.04 (packaged in imx-boot)
# ccmp15        5.15              2021.10
# ccmp13        5.15              2021.10
#

# Help variables used in recipes
HAVE_WIFI       = "${@bb.utils.contains('MACHINE_FEATURES', 'wifi', '1', '', d)}"
HAVE_BT         = "${@bb.utils.contains('MACHINE_FEATURES', 'bluetooth', '1', '', d)}"
HAVE_GUI        = "${@bb.utils.contains('DISTRO_FEATURES', 'x11', '1', '', d)}"

# Default value for WiFi virtual wlans
HAS_WIFI_VIRTWLANS ?= "false"

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
WLAN_P2P_DEVICE_NAME ??= "${DIGI_SOM}-p2p"
BT_DEVICE_NAME       ??= "${DIGI_SOM}"

# Digi BSP default settings
IMAGE_CLASSES = "image_types_digi"

# Do not enable console on VT (see sysvinit-inittab recipe)
USE_VT ?= "0"

# Extra RDEPENDS
OPTEE_PKGS ??= ""
MACHINE_EXTRA_RDEPENDS += " \
    ${@bb.utils.contains("MACHINE_FEATURES", "mca", "mca-tool", "", d)} \
    ${@bb.utils.contains('MACHINE_FEATURES', 'optee', '${OPTEE_PKGS}', '', d)} \
"
# Extra RRECOMMENDS
MACHINE_EXTRA_RRECOMMENDS += " \
    ${@bb.utils.contains("MACHINE_FEATURES", "cryptochip", "cryptoauthlib", "", d)} \
"

# Extra rules when device manager is systemd or udev
UDEV_EXTRA_RULES = "udev-extraconf"
MACHINE_EXTRA_RRECOMMENDS += "${@bb.utils.contains_any('VIRTUAL-RUNTIME_dev_manager', 'systemd udev', '${UDEV_EXTRA_RULES}', '', d)}"

# Default image name (for install scripts)
DEFAULT_IMAGE_NAME ??= "dey-image-qt"

# List of graphical images names (for install scripts)
GRAPHICAL_IMAGES ?= "dey-image-crank dey-image-qt dey-image-webkit dey-image-lvgl"

# Include DEY SELinux policy modifications by default
DEY_SELINUX_POLICY ?= "1"

# U-Boot scripts to include in 'linux' partition
# (use the '+=' operator, since other layers may append scripts to this list)
BOOT_SCRIPTS += "boot.scr:boot.scr"

# This can be used to enable U-Boot update through swupdate
SWUPDATE_UBOOTIMG ?= "false"
SWUPDATE_UBOOTIMG_REDUNDANT ?= "false"

# ConnectCore Cloud Services packages to install
CCCS_PKGS ?= "cccs-gs-demo dey-examples-cccs"

# Default 1k-bloks seek in boot partition.
BOOTLOADER_SEEK_BOOTPART ?= "0"
