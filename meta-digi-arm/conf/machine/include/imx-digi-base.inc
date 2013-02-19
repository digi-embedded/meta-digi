# Digi's i.MX platforms common settings
include conf/machine/include/digi-defaults.inc
include conf/machine/include/soc-family.inc
include conf/machine/include/tune-cortexa8.inc

# Disable perf features as 2.6.35.3 fail to build otherwise
PERF_FEATURES_ENABLE = ""

UBOOT_MAKE_TARGET = "u-boot.bin"
UBOOT_SUFFIX = "bin"
UBOOT_PADDING = "2"

# Set UBOOT_CONFIG_VARIANT in local.conf accordingly, default is empty:
# MX5X: 128sdram,ext_eth,128sdram_test,128sdram_dbg,test,dbg,test_dbg,db_pp,db_pa,db_rp,db_ra
# MX51: EAK
# MX53: 4Kpage

UBOOT_CONFIG_VARIANT ?= ""

UBOOT_MACHINE = '${MACHINE}${@base_conditional( "UBOOT_CONFIG_VARIANT", "", "_", "_${UBOOT_CONFIG_VARIANT}_", d )}config'
UBOOT_IMAGE = 'u-boot-${MACHINE}${@base_conditional( "UBOOT_CONFIG_VARIANT", "", "-", "-${UBOOT_CONFIG_VARIANT}-", d )}${PV}-${PR}.${UBOOT_SUFFIX}'
UBOOT_SYMLINK = 'u-boot-${MACHINE}${@base_conditional( "UBOOT_CONFIG_VARIANT", "", "", "-${UBOOT_CONFIG_VARIANT}", d )}.${UBOOT_SUFFIX}'

XSERVER = "xserver-xorg \
           xf86-input-evdev \
           xf86-video-imxfb"

# Ship kernel modules
MACHINE_EXTRA_RRECOMMENDS = "kernel-modules"

# Extra audio support
MACHINE_EXTRA_RRECOMMENDS += "imx-audio"

# Extra udev rules
MACHINE_EXTRA_RRECOMMENDS += "udev-extraconf"

# Gstreamer plugin
MACHINE_GSTREAMER_PLUGIN ?= "gst-fsl-plugin"

PREFERRED_PROVIDER_virtual/egl_mx5      ?= "${@base_contains('DISTRO_FEATURES', 'x11', 'amd-gpu-x11-bin-mx51', 'amd-gpu-bin-mx51', d)}"
PREFERRED_PROVIDER_virtual/libgles1_mx5 ?= "${@base_contains('DISTRO_FEATURES', 'x11', 'amd-gpu-x11-bin-mx51', 'amd-gpu-bin-mx51', d)}"
PREFERRED_PROVIDER_virtual/libgles2_mx5 ?= "${@base_contains('DISTRO_FEATURES', 'x11', 'amd-gpu-x11-bin-mx51', 'amd-gpu-bin-mx51', d)}"

EXTRA_IMAGEDEPENDS = "u-boot"

# Flash image types
SDCARD_ROOTFS ?= "${DEPLOY_DIR_IMAGE}/${IMAGE_NAME}.rootfs.ext3"
IMAGE_FSTYPES ?= "ext3 sdcard sum.jffs2 tar.bz2 ubifs"

KERNEL_IMAGETYPE = "uImage"

MACHINE_FEATURES = "apm usbgadget usbhost vfat alsa touchscreen"
