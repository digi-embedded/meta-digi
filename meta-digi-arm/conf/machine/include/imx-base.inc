# Provides the i.MX architecture common settings

include conf/machine/include/digi-defaults.inc
include conf/machine/include/soc-family.inc

# [DIGI] [TODO] Don't think we need this.
# Disable perf features as 2.6.35.3 fail to build otherwise
# PERF_FEATURES_ENABLE = ""

# [DIGI] TODO need to understand this.
# Set specific make target and binary suffix
UBOOT_MAKE_TARGET = "u-boot.imx"
UBOOT_SUFFIX = "imx"

UBOOT_ENTRYPOINT_mx51  = "0x94000000"
UBOOT_LOADADDRESS_mx51 = "0x94000000"

UBOOT_ENTRYPOINT_mx53  = "0x70800000"
UBOOT_LOADADDRESS_mx53 = "0x70800000"

# [DIGI] Work in progress
# UBOOT_ENTRYPOINT_mx6  = "0x10800000"
# UBOOT_LOADADDRESS_mx6 = "0x10800000"

XSERVER_DRIVER = "xf86-video-fbdev"
XSERVER_DRIVER_mx5 = "xf86-video-imxfb"
XSERVER = "xserver-xorg \
           xf86-input-evdev \
           ${XSERVER_DRIVER}"

# Ship kernel modules
MACHINE_EXTRA_RRECOMMENDS = "kernel-modules"

# Firmware
MACHINE_FIRMWARE ?= ""
MACHINE_FIRMWARE_mx53 = "firmware-imx-vpu-imx53 firmware-imx-sdma-imx53"
MACHINE_FIRMWARE_mx51 = "firmware-imx-vpu-imx51 firmware-imx-sdma-imx51"

MACHINE_EXTRA_RRECOMMENDS += "${MACHINE_FIRMWARE}"

# Extra audio support
MACHINE_EXTRA_RRECOMMENDS += "imx-audio \
                              imx-lib \
                              udev-extraconf"

# Gstreamer plugin
MACHINE_GSTREAMER_PLUGIN ?= "gst-fsl-plugin"

# [DIGI] TODO This may be amd-gpu-bin-mx51
PREFERRED_PROVIDER_virtual/egl_mx5 ?= "amd-gpu-x11-bin-mx51"
PREFERRED_PROVIDER_virtual/libgles1_mx5 ?= "amd-gpu-x11-bin-mx51"
PREFERRED_PROVIDER_virtual/libgles2_mx5 ?= "amd-gpu-x11-bin-mx51"

EXTRA_IMAGEDEPENDS = "u-boot"

SDCARD_ROOTFS ?= "${DEPLOY_DIR_IMAGE}/${IMAGE_NAME}.rootfs.ext3"
IMAGE_FSTYPES ?= "tar.bz2 ext3 sdcard"

SERIAL_CONSOLE = "115200 ttymxc0"

KERNEL_IMAGETYPE = "uImage"

MACHINE_FEATURES = "apm usbgadget usbhost vfat alsa touchscreen"
