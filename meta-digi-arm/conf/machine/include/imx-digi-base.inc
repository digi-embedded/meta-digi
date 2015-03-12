# Digi's i.MX platforms common settings
include conf/machine/include/digi-defaults.inc
include conf/machine/include/soc-family.inc

XSERVER_DRIVER = "xf86-video-fbdev"
XSERVER_DRIVER_mx5 = "xf86-video-imxfb"
XSERVER_DRIVER_mx6 = "xf86-video-imxfb-vivante"
XSERVER = "xserver-xorg \
           xf86-input-evdev \
           ${XSERVER_DRIVER}"

PREFERRED_VERSION_directfb_mx6 = "1.7.4"

# Ship kernel modules
MACHINE_EXTRA_RRECOMMENDS = "kernel-modules"

# Float-Point setting
# handled by software
# DEFAULTTUNE_mx6 ?= "cortexa9-neon"
# handled by hardware
DEFAULTTUNE_mx6 ?= "cortexa9hf-neon"

# Sub-architecture support
MACHINE_SOCARCH ?= ""
MACHINE_SOCARCH_mx5 = "${TUNE_PKGARCH}-mx5"
MACHINE_SOCARCH_mx6q = "${TUNE_PKGARCH}-mx6qdl"
MACHINE_SOCARCH_mx6dl = "${TUNE_PKGARCH}-mx6qdl"
PACKAGE_EXTRA_ARCHS_append = " ${MACHINE_SOCARCH}"

MACHINE_ARCH_FILTER = "virtual/kernel"
MACHINE_SOCARCH_FILTER = "libfslcodec libfslvpuwrap libfslparser gst-fsl-plugins"
MACHINE_SOCARCH_FILTER_append_mx5 = " virtual/libgles1 virtual/libgles2 virtual/egl virtual/mesa virtual/libgl"
MACHINE_SOCARCH_FILTER_append_mx6 = " virtual/libopenvg virtual/libgles1 virtual/libgles2 virtual/egl virtual/mesa virtual/libgl virtual/libg2d"

INHERIT += "fsl-dynamic-packagearch"

# Extra udev rules
MACHINE_EXTRA_RRECOMMENDS += "udev-extraconf udev-rules-imx"

# GStreamer 0.10 plugins
MACHINE_GSTREAMER_PLUGIN ?= "gst-fsl-plugin"

# GStreamer 1.0 plugins
MACHINE_GSTREAMER_1_0_PLUGIN ?= ""
MACHINE_GSTREAMER_1_0_PLUGIN_mx6q ?= "gstreamer1.0-plugins-imx-meta"
MACHINE_GSTREAMER_1_0_PLUGIN_mx6dl ?= "gstreamer1.0-plugins-imx-meta"

# mx5 GPU libraries
GPU_FLAVOUR = "${@base_contains('DISTRO_FEATURES', 'x11', 'x11-', '', d)}"
PREFERRED_PROVIDER_virtual/egl_mx5 ?= "amd-gpu-${GPU_FLAVOUR}bin-mx51"
PREFERRED_PROVIDER_virtual/libgles1_mx5 ?= "amd-gpu-${GPU_FLAVOUR}bin-mx51"
PREFERRED_PROVIDER_virtual/libgles2_mx5 ?= "amd-gpu-${GPU_FLAVOUR}bin-mx51"

# mx6 GPU libraries
PREFERRED_PROVIDER_virtual/egl_mx6 ?= "imx-gpu-viv"
PREFERRED_PROVIDER_virtual/libgles1_mx6q ?= "imx-gpu-viv"
PREFERRED_PROVIDER_virtual/libgles1_mx6dl ?= "imx-gpu-viv"
PREFERRED_PROVIDER_virtual/libgles2_mx6q ?= "imx-gpu-viv"
PREFERRED_PROVIDER_virtual/libgles2_mx6dl ?= "imx-gpu-viv"
PREFERRED_PROVIDER_virtual/libgl_mx6q ?= "imx-gpu-viv"
PREFERRED_PROVIDER_virtual/libgl_mx6dl ?= "imx-gpu-viv"
PREFERRED_PROVIDER_virtual/libg2d_mx6 ?= "imx-gpu-viv"

EXTRA_IMAGEDEPENDS = "u-boot"

# Flash image types
IMAGE_FSTYPES ??= "jffs2.sum tar.bz2 ubifs"

# MX6 does not have flash memory so build sdcard images
SDCARD_ROOTFS_mx6 ?= "${DEPLOY_DIR_IMAGE}/${IMAGE_NAME}.rootfs.ext4"

KERNEL_IMAGETYPE = "uImage"

MACHINE_FEATURES = "apm usbgadget usbhost vfat alsa touchscreen"
