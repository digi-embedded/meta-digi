# Digi's i.MX platforms common settings
include conf/machine/include/digi-defaults.inc
include conf/machine/include/soc-family.inc
include conf/machine/include/tune-arm926ejs.inc

# Disable perf features as 2.6.35.3 fail to build otherwise
PERF_FEATURES_ENABLE = ""

XSERVER = "xserver-xorg \
           xf86-input-evdev \
           xf86-video-fbdev"

# Ship kernel modules
MACHINE_EXTRA_RRECOMMENDS = "kernel-modules"

EXTRA_IMAGEDEPENDS = "u-boot"

# Flash image types
# [TODO] add SDCARD support
# At the moment it's failing due to wrong u-boot image
# SDCARD_ROOTFS ?= "${DEPLOY_DIR_IMAGE}/${IMAGE_NAME}.rootfs.ext3"
# IMAGE_FSTYPES ?= "ext3 sdcard sum.jffs2 tar.bz2 ubifs"
#
# JFFS2 is not supported by kernel 3.10, so add it only for 2.6.35
IMAGE_FSTYPES ?= "${@base_conditional('IS_KERNEL_2X', '1' , 'sum.jffs2', '', d)} tar.bz2 ubifs"

KERNEL_IMAGETYPE = "uImage"

MACHINE_FEATURES = "apm usbgadget usbhost vfat"
