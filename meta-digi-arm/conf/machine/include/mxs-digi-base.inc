# Digi's MXS platforms common settings
include conf/machine/include/digi-defaults.inc
include conf/machine/include/soc-family.inc
include conf/machine/include/tune-arm926ejs.inc

# Disable perf features as 2.6.35.3 fail to build otherwise
PERF_FEATURES_ENABLE_mx28 = ""

XSERVER = "xserver-xorg \
           xf86-input-evdev \
           xf86-video-fbdev"

# Gstreamer plugin
MACHINE_GSTREAMER_PLUGIN ?= ""

# Sub-architecture support
MACHINE_SOCARCH ?= ""
MACHINE_SOCARCH_mx28 = "${TUNE_PKGARCH}-mx28"
PACKAGE_EXTRA_ARCHS_append = " ${MACHINE_SOCARCH}"

MACHINE_ARCH_FILTER = "virtual/kernel"
MACHINE_SOCARCH_FILTER = "libfslcodec libfslvpuwrap libfslparser gst-fsl-plugins"

INHERIT += "fsl-dynamic-packagearch"

# Ship kernel modules
MACHINE_EXTRA_RRECOMMENDS = "kernel-modules"

# Extra udev rules
MACHINE_EXTRA_RRECOMMENDS += "udev-extraconf"

EXTRA_IMAGEDEPENDS = "u-boot"

# Flash image types
IMAGE_FSTYPES ?= "jffs2.sum tar.bz2 ubifs"

KERNEL_IMAGETYPE = "uImage"

MACHINE_FEATURES = "apm usbgadget usbhost vfat"
