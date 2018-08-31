# Digi's i.MX platforms common settings
include conf/machine/include/digi-defaults.inc

XSERVER_DRIVER = "xf86-video-fbdev"
XSERVER_DRIVER_imxgpu2d = "xf86-video-imx-vivante"
XSERVER_DRIVER_append_mx8 = " xf86-video-modesetting"
XSERVER = "xserver-xorg \
           xf86-input-evdev \
           ${XSERVER_DRIVER}"

# Ship kernel modules
MACHINE_EXTRA_RRECOMMENDS = "kernel-modules"

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

MACHINEOVERRIDES_EXTENDER_mx6q   = "imxfbdev:imxipu:imxvpu:imxvpucnm:imxgpu:imxgpu2d:imxdpublit:imxgpu3d"
MACHINEOVERRIDES_EXTENDER_mx6dl  = "imxfbdev:imxipu:imxpxp:imxvpu:imxvpucnm:imxgpu:imxgpu2d:imxdpublit:imxgpu3d:imxepdc"
MACHINEOVERRIDES_EXTENDER_mx6ul  = "imxfbdev:imxpxp"
MACHINEOVERRIDES_EXTENDER_mx8qxp = "imxdrm:imxdpu:imxgpu:imxgpu2d:imxgpu3d"

# Sub-architecture support
MACHINE_SOCARCH_SUFFIX ?= ""
MACHINE_SOCARCH_SUFFIX_mx6q = "-mx6qdl"
MACHINE_SOCARCH_SUFFIX_mx6dl = "-mx6qdl"
MACHINE_SOCARCH_SUFFIX_mx6ul = "-mx6ul"
MACHINE_SOCARCH_SUFFIX_mx8qxp = "-mx8qxp"
MACHINE_SOCARCH_SUFFIX_mx8dxp = "-mx8dxp"
MACHINE_SOCARCH_SUFFIX_mx8dx = "-mx8dx"

MACHINE_ARCH_FILTER = "virtual/kernel"
MACHINE_SOCARCH_FILTER_append_imxvpu = " imx-vpu libimxvpuapi imx-codec imx-vpuwrap imx-parser"
MACHINE_SOCARCH_FILTER_append_imxgpu2d = " virtual/libopenvg virtual/libgles1 virtual/libgles2 virtual/egl virtual/mesa virtual/libgl virtual/libg2d cairo pango"
MACHINE_SOCARCH_FILTER_append_imxpxp = " imx-codec imx-parser"
MACHINE_SOCARCH_FILTER_append_mx8 = " \
    virtual/libopenvx \
    virtual/opencl-icd \
    opencl-headers \
"

INHERIT += "fsl-dynamic-packagearch"

SIGGEN_EXCLUDE_SAFE_RECIPE_DEPS_append = " \
    imx-gpu-viv->kernel-module-imx-gpu-viv \
    libimxvpuapi->imx-vpu \
    imx-vpuwrap->imx-vpu \
    imx-codec->imx-vpu \
    imx-test->imx-vpu \
"

# Extra udev rules
MACHINE_EXTRA_RRECOMMENDS += "udev-extraconf udev-rules-imx"

# GStreamer 1.0 plugins
MACHINE_GSTREAMER_1_0_PLUGIN ?= ""
MACHINE_GSTREAMER_1_0_PLUGIN_mx6q ?= "imx-gst1.0-plugin imx-gst1.0-plugin-gplay imx-gst1.0-plugin-grecorder"
MACHINE_GSTREAMER_1_0_PLUGIN_mx6dl ?= "imx-gst1.0-plugin imx-gst1.0-plugin-gplay imx-gst1.0-plugin-grecorder"
MACHINE_GSTREAMER_1_0_PLUGIN_mx6ul ?= "imx-gst1.0-plugin imx-gst1.0-plugin-gplay imx-gst1.0-plugin-grecorder"
MACHINE_GSTREAMER_1_0_PLUGIN_mx8 ?= "imx-gst1.0-plugin imx-gst1.0-plugin-gplay imx-gst1.0-plugin-grecorder"

# Determines if the SoC has support for Vivante kernel driver
SOC_HAS_VIVANTE_KERNEL_DRIVER_SUPPORT       = "0"
SOC_HAS_VIVANTE_KERNEL_DRIVER_SUPPORT_mx6q  = "1"
SOC_HAS_VIVANTE_KERNEL_DRIVER_SUPPORT_mx6dl = "1"
SOC_HAS_VIVANTE_KERNEL_DRIVER_SUPPORT_mx8   = "1"

# Handle Vivante kernel driver setting:
#   0 - machine does not have Vivante GPU driver support
#   1 - machine has Vivante GPU driver support
MACHINE_HAS_VIVANTE_KERNEL_DRIVER_SUPPORT ?= "${SOC_HAS_VIVANTE_KERNEL_DRIVER_SUPPORT}"

# mx6 GPU libraries
PREFERRED_PROVIDER_virtual/egl_mx6q ?= "imx-gpu-viv"
PREFERRED_PROVIDER_virtual/egl_mx6dl ?= "imx-gpu-viv"
PREFERRED_PROVIDER_virtual/egl_mx6ul ?= "mesa"
PREFERRED_PROVIDER_virtual/egl_mx8 ?= "imx-gpu-viv"

PREFERRED_PROVIDER_virtual/libgles1_mx6q ?= "imx-gpu-viv"
PREFERRED_PROVIDER_virtual/libgles1_mx6dl ?= "imx-gpu-viv"
PREFERRED_PROVIDER_virtual/libgles1_mx6ul ?= "mesa"
PREFERRED_PROVIDER_virtual/libgles1_mx8 ?= "imx-gpu-viv"

PREFERRED_PROVIDER_virtual/libgles2_mx6q ?= "imx-gpu-viv"
PREFERRED_PROVIDER_virtual/libgles2_mx6dl ?= "imx-gpu-viv"
PREFERRED_PROVIDER_virtual/libgles2_mx6ul ?= "mesa"
PREFERRED_PROVIDER_virtual/libgles2_mx8 ?= "imx-gpu-viv"

PREFERRED_PROVIDER_virtual/libgl_mx6q ?= "imx-gpu-viv"
PREFERRED_PROVIDER_virtual/libgl_mx6dl ?= "imx-gpu-viv"
PREFERRED_PROVIDER_virtual/libgl_mx6ul ?= "mesa"
PREFERRED_PROVIDER_virtual/libgl_mx8 ?= "imx-gpu-viv"

PREFERRED_PROVIDER_virtual/libg2d_mx6 ?= "imx-gpu-viv"
PREFERRED_PROVIDER_virtual/libg2d_mx6ul = ""
PREFERRED_PROVIDER_virtual/libg2d_mx8 = "imx-dpu-g2d"

EXTRA_IMAGEDEPENDS = "u-boot"

KERNEL_IMAGETYPE ?= "zImage"

MACHINE_FEATURES = "usbgadget usbhost vfat alsa touchscreen"

# Add the ability to specify _imx machines
MACHINEOVERRIDES =. "imx:"
