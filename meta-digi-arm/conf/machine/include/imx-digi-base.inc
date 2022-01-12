# Digi's i.MX platforms common settings
include conf/machine/include/digi-defaults.inc

XSERVER_DRIVER = "xf86-video-fbdev"
XSERVER_DRIVER_imxgpu2d = "xf86-video-imx-vivante"
XSERVER_DRIVER_append_mx8 = " xf86-video-modesetting"
XSERVER = "xserver-xorg \
           xf86-input-evdev \
           ${XSERVER_DRIVER}"

# Ship kernel modules
MACHINE_EXTRA_RRECOMMENDS += "kernel-modules"

# Tunes for hard/soft float-point selection. Note that we allow building for
# thumb support giving distros the chance to enable thumb by setting
# ARM_INSTRUCTION_SET = "thumb"
#
# handled by software
# DEFAULTTUNE_mx6 ?= "cortexa9t-neon"
# handled by hardware
DEFAULTTUNE_mx6 ?= "cortexa9thf-neon"
DEFAULTTUNE_mx6ul ?= "cortexa7thf-neon"
DEFAULTTUNE_mx8 ?= "aarch64"

INHERIT += "machine-overrides-extender"

MACHINEOVERRIDES_EXTENDER_mx6q   = "imxfbdev:imxipu:imxvpu:imxgpu:imxgpu2d:imxgpu3d"
MACHINEOVERRIDES_EXTENDER_mx6dl  = "imxfbdev:imxpxp:imxipu:imxvpu:imxgpu:imxgpu2d:imxgpu3d:imxepdc"
MACHINEOVERRIDES_EXTENDER_mx6ul  = "imxfbdev:imxpxp"
MACHINEOVERRIDES_EXTENDER_mx8qxp = "imxdrm:imxdpu:imxgpu:imxgpu2d:imxgpu3d"
MACHINEOVERRIDES_EXTENDER_mx8mn  = "imxdrm:imxgpu:imxgpu3d"
MACHINEOVERRIDES_EXTENDER_mx8mm  = "imxdrm:imxvpu:imxgpu:imxgpu2d:imxgpu3d"
MACHINEOVERRIDES_EXTENDER_mx8mp  = "imxdrm:imxvpu:imxgpu:imxgpu2d:imxgpu3d"

# Sub-architecture support
MACHINE_SOCARCH_SUFFIX ?= ""
MACHINE_SOCARCH_SUFFIX_mx6q = "-mx6qdl"
MACHINE_SOCARCH_SUFFIX_mx6dl = "-mx6qdl"
MACHINE_SOCARCH_SUFFIX_mx6ul = "-mx6ul"
MACHINE_SOCARCH_SUFFIX_mx8qxp = "-mx8qxp"
MACHINE_SOCARCH_SUFFIX_mx8dxp = "-mx8dxp"
MACHINE_SOCARCH_SUFFIX_mx8dx = "-mx8dx"
MACHINE_SOCARCH_SUFFIX_mx8mn  = "-mx8mn"
MACHINE_SOCARCH_SUFFIX_mx8mm  = "-mx8mm"
MACHINE_SOCARCH_SUFFIX_mx8mp  = "-mx8mp"

MACHINE_ARCH_FILTER = "virtual/kernel"
MACHINE_SOCARCH_FILTER_append_imxvpu = " \
    imx-codec \
    imx-vpuwrap \
    libimxvpuapi \
    virtual/imxvpu \
"
MACHINE_SOCARCH_FILTER_append_imxgpu = " \
    virtual/egl \
    virtual/mesa \
    virtual/libopenvg \
    libdrm \
    cairo \
    opencv \
    libgal-imx \
    pango \
"
MACHINE_SOCARCH_FILTER_append_imxgpu2d = " \
    virtual/libg2d \
"
MACHINE_SOCARCH_FILTER_append_imxgpu3d = " \
    virtual/libgl \
    virtual/libgles1 \
    virtual/libgles2 \
"
MACHINE_SOCARCH_FILTER_append_mx6q = " \
    virtual/opencl-icd \
    opencl-headers \
"
MACHINE_SOCARCH_FILTER_append_mx8 = " \
    virtual/opencl-icd \
    opencl-headers \
"
MACHINE_SOCARCH_FILTER_append_mx8qm = " \
    virtual/libopenvx \
"

INHERIT += "fsl-dynamic-packagearch"

SIGGEN_EXCLUDE_SAFE_RECIPE_DEPS_append = " \
    libimxvpuapi->virtual/imxvpu \
    imx-vpuwrap->virtual/imxvpu \
    imx-codec->virtual/imxvpu \
    imx-test->virtual/imxvpu \
"

# Extra udev rules
MACHINE_EXTRA_RRECOMMENDS += "udev-extraconf udev-rules-imx"

# GStreamer 1.0 plugins
MACHINE_GSTREAMER_1_0_PLUGIN ?= ""
MACHINE_GSTREAMER_1_0_PLUGIN_use-mainline-bsp ?= "gstreamer1.0-plugins-imx-meta"
MACHINE_GSTREAMER_1_0_PLUGIN_mx6q ?= "imx-gst1.0-plugin imx-gst1.0-plugin-gplay imx-gst1.0-plugin-grecorder"
MACHINE_GSTREAMER_1_0_PLUGIN_mx6dl ?= "imx-gst1.0-plugin imx-gst1.0-plugin-gplay imx-gst1.0-plugin-grecorder"
MACHINE_GSTREAMER_1_0_PLUGIN_mx6ul ?= "imx-gst1.0-plugin imx-gst1.0-plugin-gplay imx-gst1.0-plugin-grecorder"
MACHINE_GSTREAMER_1_0_PLUGIN_mx8 ?= "imx-gst1.0-plugin imx-gst1.0-plugin-gplay imx-gst1.0-plugin-grecorder"

# Determines if the SoC has support for Vivante kernel driver
SOC_HAS_VIVANTE_KERNEL_DRIVER_SUPPORT        = "0"
SOC_HAS_VIVANTE_KERNEL_DRIVER_SUPPORT_imxgpu = "1"

# Handle Vivante kernel driver setting:
#   0 - machine does not have Vivante GPU driver support
#   1 - machine has Vivante GPU driver support
MACHINE_HAS_VIVANTE_KERNEL_DRIVER_SUPPORT ?= "${SOC_HAS_VIVANTE_KERNEL_DRIVER_SUPPORT}"

# Graphics libraries
PREFERRED_PROVIDER_virtual/egl      ?= "mesa"
PREFERRED_PROVIDER_virtual/libgl    ?= "mesa"
PREFERRED_PROVIDER_virtual/libgles1 ?= "mesa"
PREFERRED_PROVIDER_virtual/libgles2 ?= "mesa"

PREFERRED_PROVIDER_virtual/egl_imxgpu        ?= "imx-gpu-viv"
PREFERRED_PROVIDER_virtual/libgl_imxgpu3d    ?= "imx-gpu-viv"
PREFERRED_PROVIDER_virtual/libgles1_imxgpu3d ?= "imx-gpu-viv"
PREFERRED_PROVIDER_virtual/libgles2_imxgpu3d ?= "imx-gpu-viv"
PREFERRED_PROVIDER_virtual/libg2d            ?= "imx-gpu-g2d"
PREFERRED_PROVIDER_virtual/libg2d_imxdpu     ?= "imx-dpu-g2d"

EXTRA_IMAGEDEPENDS = "u-boot"

KERNEL_IMAGETYPE ?= "zImage"

MACHINE_FEATURES = "usbgadget usbhost vfat alsa touchscreen"

# Add the ability to specify _imx machines
MACHINEOVERRIDES =. "imx:"

# Add the ability to use exclusive recipes for NXP BSP
MACHINEOVERRIDES =. "use-nxp-bsp:"

# U-Boot 2017.X requires a specific dtc version.
PREFERRED_VERSION_dtc-native_ccimx6 ?= "1.4.5"
PREFERRED_VERSION_nativesdk-dtc_ccimx6 ?= "1.4.5"

DEY_BUILD_PLATFORM = "NXP"
