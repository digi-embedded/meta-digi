# Digi's i.MX platforms common settings
include conf/machine/include/digi-defaults.inc
include conf/machine/include/soc-family.inc
include conf/machine/include/tune-cortexa8.inc

# Disable perf features as 2.6.35.3 fail to build otherwise
PERF_FEATURES_ENABLE = ""

XSERVER = "xserver-xorg \
           xf86-input-evdev \
           xf86-video-imxfb"

# Ship kernel modules
MACHINE_EXTRA_RRECOMMENDS = "kernel-modules"

# Extra audio support
MACHINE_EXTRA_RRECOMMENDS += "imx-audio"

# Gstreamer plugin
MACHINE_GSTREAMER_PLUGIN ?= "gst-fsl-plugin"

# MX5 GPU libraries
GPU_FLAVOUR = "${@base_contains('DISTRO_FEATURES', 'x11', 'x11-', '', d)}"
PREFERRED_PROVIDER_virtual/egl_mx5      ?= "amd-gpu-${GPU_FLAVOUR}bin-mx51"
PREFERRED_PROVIDER_virtual/libgles1_mx5 ?= "amd-gpu-${GPU_FLAVOUR}bin-mx51"
PREFERRED_PROVIDER_virtual/libgles2_mx5 ?= "amd-gpu-${GPU_FLAVOUR}bin-mx51"

EXTRA_IMAGEDEPENDS = "u-boot"

# Flash image types
# Do not build ext3/sdcard until they are tested.
#SDCARD_ROOTFS ?= "${DEPLOY_DIR_IMAGE}/${IMAGE_NAME}.rootfs.ext3"
#IMAGE_FSTYPES ?= "ext3 sdcard sum.jffs2 tar.bz2 ubifs"
IMAGE_FSTYPES ?= "sum.jffs2 tar.bz2 ubifs"

KERNEL_IMAGETYPE = "uImage"

MACHINE_FEATURES = "apm usbgadget usbhost vfat alsa touchscreen"
