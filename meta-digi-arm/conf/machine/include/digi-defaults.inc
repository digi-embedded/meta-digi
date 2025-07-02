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
# ccimx6        5.15              2017.03
# ccimx6qp      5.15              2017.03
# ccimx6ul      6.6               2024.04
# ccimx8x       6.6               2024.04 (packaged in imx-boot)
# ccimx8mn      6.6               2024.04 (packaged in imx-boot)
# ccimx8mm      6.6               2024.04 (packaged in imx-boot)
# ccimx91       6.6               2024.04 (packaged in imx-boot)
# ccimx93       6.6               2024.04 (packaged in imx-boot)
# ccmp15        6.6               2023.10
# ccmp13        6.6               2023.10
# ccmp25        6.6               2023.10
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

# Remove IMAGE_NAME_SUFFIX from IMAGE_LINK_NAME so that our image name format
# matches the ones used in old DEY versions
IMAGE_LINK_NAME = "${IMAGE_BASENAME}${IMAGE_MACHINE_SUFFIX}"

# List of graphical images names (for install scripts)
GRAPHICAL_IMAGES ?= "dey-image-qt dey-image-webkit dey-image-lvgl"

# Default wayland socket
WAYLAND_DISPLAY ?= "wayland-1"

# Include DEY SELinux policy modifications by default
DEY_SELINUX_POLICY ?= "1"

# U-Boot scripts to include in 'linux' partition
# (use the '+=' operator, since other layers may append scripts to this list)
BOOT_SCRIPTS += "boot.scr:boot.scr"

# Select internal or Github Linux repo
LINUX_URI_STASH = "${DIGI_MTK_GIT}/linux/linux.git;protocol=ssh"
LINUX_URI_GITHUB = "${DIGI_GITHUB_GIT}/linux.git;protocol=https"
LINUX_GIT_URI ?= "${@oe.utils.conditional('DIGI_INTERNAL_GIT', '1', '${LINUX_URI_STASH}', '${LINUX_URI_GITHUB}', d)}"

# This can be used to enable U-Boot update through swupdate
SWUPDATE_UBOOTIMG ?= "false"
SWUPDATE_UBOOTIMG_REDUNDANT ?= "false"

# ConnectCore Cloud Services packages to install
CCCS_PKGS ?= "cccs-gs-demo dey-examples-cccs"

# Default 1k-bloks seek in boot partition.
BOOTLOADER_SEEK_BOOTPART ?= "0"

# Default all platforms to use /root as the home root folder.
# This is otherwise forced by systemd because of hard-coded paths
# in systemd source code.
ROOT_HOME ?= "/root"

# Default IMAGE_FSTYPES for eMMC/NAND
IMAGE_FSTYPES ?= "${@ \
	oe.utils.conditional('STORAGE_MEDIA', 'mmc', \
		bb.utils.contains('IMAGE_FEATURES', 'read-only-rootfs', \
			'boot.vfat recovery.vfat squashfs tar.xz', \
			'boot.vfat recovery.vfat ext4.gz tar.xz', d), \
	oe.utils.conditional('STORAGE_MEDIA', 'mtd', \
		bb.utils.contains('IMAGE_FEATURES', 'read-only-rootfs', \
			'boot.ubifs recovery.ubifs squashfs tar.xz', \
			'boot.ubifs recovery.ubifs ubifs tar.xz', d), \
		'' , d), \
	d) \
}"
