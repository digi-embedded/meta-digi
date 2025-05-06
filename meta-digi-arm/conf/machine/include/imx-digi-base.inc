# Digi's i.MX platforms common settings
include conf/machine/include/digi-defaults.inc

# @TODO: Use NXP's base file instead of copying
# IMX_DEFAULT_BSP = "nxp"
# include conf/machine/include/imx-base.inc

# Machines or distros can define which BSP it should use by default. We are
# intending to default for mainline BSP by default and specific machines or
# DISTROs might change it if need.
#
# Two values are considered valid: mainline, nxp
IMX_DEFAULT_BSP ?= "nxp"
MACHINEOVERRIDES =. "use-${IMX_DEFAULT_BSP}-bsp:"

# Allow setting the UART used during the boot by ATF.

# FIXME: We should return INVALID here but currently only i.MX8M has support to override the UART
# base address in source code.
SOC_ATF_BOOT_UART_BASE                    = ""

SOC_ATF_BOOT_UART_BASE:mx8m-generic-bsp   = "0x30860000"
ATF_BOOT_UART_BASE                       ?= "${SOC_ATF_BOOT_UART_BASE}"

PREFERRED_PROVIDER_virtual/xserver      = "xserver-xorg"
XSERVER_DRIVER                          = "xf86-video-fbdev"
XSERVER_DRIVER:vf-generic-bsp           = "xf86-video-modesetting"
XSERVER_DRIVER:append:mx8-generic-bsp   = " xf86-video-modesetting"
XSERVER = "xserver-xorg \
           xf86-input-evdev \
           ${XSERVER_DRIVER}"

# Ship kernel modules
MACHINE_EXTRA_RRECOMMENDS += "kernel-modules"

# DEFAULTTUNE values ported form meta-freescale (imx-base.inc)
# Tunes for hard/soft float-point selection. Note that we allow building for
# thumb support giving distros the chance to enable thumb by setting
# ARM_INSTRUCTION_SET = "thumb"
#
# handled by software
# DEFAULTTUNE:mx6-generic-bsp ?= "cortexa9t-neon"
# handled by hardware
DEFAULTTUNE:mx6-generic-bsp    ?= "cortexa9thf-neon"
DEFAULTTUNE:mx6ul-generic-bsp  ?= "cortexa7thf-neon"
DEFAULTTUNE:mx8m-generic-bsp   ?= "cortexa53-crypto"
DEFAULTTUNE:mx8x-generic-bsp   ?= "cortexa35-crypto"
DEFAULTTUNE:mx91-generic-bsp   ?= "cortexa55"
DEFAULTTUNE:mx93-generic-bsp   ?= "cortexa55"

# DEFAULTTUNE values ported form meta-imx (fsl-imx-base.inc)
# Set a more generic tuning for code reuse across parts
DEFAULTTUNE:mx8m-nxp-bsp   ?= "armv8a-crc-crypto"
DEFAULTTUNE:mx8x-nxp-bsp   ?= "armv8a-crc-crypto"
DEFAULTTUNE:mx91-nxp-bsp   ?= "armv8a-crc-crypto"
DEFAULTTUNE:mx93-nxp-bsp   ?= "armv8a-crc-crypto"

INHERIT += "machine-overrides-extender"

#######
### NXP BSP specific overrides
#######
MACHINEOVERRIDES_EXTENDER:mx6q:use-nxp-bsp   = "imx-generic-bsp:imx-nxp-bsp:imxfbdev:imxipu:imxvpu:imxgpu:imxviv:imxgpu2d:imxgpu3d:mx6-generic-bsp:mx6-nxp-bsp:mx6q-generic-bsp:mx6q-nxp-bsp"
MACHINEOVERRIDES_EXTENDER:mx6dl:use-nxp-bsp  = "imx-generic-bsp:imx-nxp-bsp:imxfbdev:imxpxp:imxipu:imxvpu:imxgpu:imxviv:imxgpu2d:imxgpu3d:imxepdc:mx6-generic-bsp:mx6-nxp-bsp:mx6dl-generic-bsp:mx6dl-nxp-bsp"
MACHINEOVERRIDES_EXTENDER:mx6ul:use-nxp-bsp  = "imx-generic-bsp:imx-nxp-bsp:imxfbdev:imxpxp:mx6-generic-bsp:mx6-nxp-bsp:mx6ul-generic-bsp:mx6ul-nxp-bsp"
MACHINEOVERRIDES_EXTENDER:mx8mm:use-nxp-bsp  = "imx-generic-bsp:imx-nxp-bsp:imxdrm:imxvpu:imxgpu:imxviv:imxgpu2d:imxgpu3d:mx8-generic-bsp:mx8-nxp-bsp:mx8m-generic-bsp:mx8m-nxp-bsp:mx8mm-generic-bsp:mx8mm-nxp-bsp"
MACHINEOVERRIDES_EXTENDER:mx8mn:use-nxp-bsp  = "imx-generic-bsp:imx-nxp-bsp:imxdrm:imxgpu:imxviv:imxgpu3d:imxvulkan:mx8-generic-bsp:mx8-nxp-bsp:mx8m-generic-bsp:mx8m-nxp-bsp:mx8mn-generic-bsp:mx8mn-nxp-bsp"
MACHINEOVERRIDES_EXTENDER:mx8mp:use-nxp-bsp  = "imx-generic-bsp:imx-nxp-bsp:imxdrm:imxvpu:imxgpu:imxviv:imxgpu2d:imxgpu3d:imxvulkan:mx8-generic-bsp:mx8-nxp-bsp:mx8m-generic-bsp:mx8m-nxp-bsp:mx8mp-generic-bsp:mx8mp-nxp-bsp"
MACHINEOVERRIDES_EXTENDER:mx8qxp:use-nxp-bsp = "imx-generic-bsp:imx-nxp-bsp:imxdrm:imxdpu:imxgpu:imxviv:imxgpu2d:imxgpu3d:imxvulkan:mx8-generic-bsp:mx8-nxp-bsp:mx8x-generic-bsp:mx8x-nxp-bsp:mx8qxp-generic-bsp:mx8qxp-nxp-bsp"
MACHINEOVERRIDES_EXTENDER:mx8dx:use-nxp-bsp  = "imx-generic-bsp:imx-nxp-bsp:imxdrm:imxdpu:imxgpu:imxviv:imxgpu2d:imxgpu3d:imxvulkan:mx8-generic-bsp:mx8-nxp-bsp:mx8x-generic-bsp:mx8x-nxp-bsp:mx8dx-generic-bsp:mx8dx-nxp-bsp"
MACHINEOVERRIDES_EXTENDER:mx91:use-nxp-bsp   = "imx-generic-bsp:imx-nxp-bsp:imxdrm:imxpxp:mx9-generic-bsp:mx9-nxp-bsp:mx91-generic-bsp:mx91-nxp-bsp"
MACHINEOVERRIDES_EXTENDER:mx93:use-nxp-bsp   = "imx-generic-bsp:imx-nxp-bsp:imxdrm:imxpxp:mx9-generic-bsp:mx9-nxp-bsp:mx93-generic-bsp:mx93-nxp-bsp"

#######
### Mainline BSP specific overrides
#######
MACHINEOVERRIDES_EXTENDER:mx6q:use-mainline-bsp   = "imx-generic-bsp:imx-mainline-bsp:mx6-generic-bsp:mx6-mainline-bsp:mx6q-generic-bsp:mx6q-mainline-bsp"
MACHINEOVERRIDES_EXTENDER:mx6dl:use-mainline-bsp  = "imx-generic-bsp:imx-mainline-bsp:mx6-generic-bsp:mx6-mainline-bsp:mx6dl-generic-bsp:mx6dl-mainline-bsp"
MACHINEOVERRIDES_EXTENDER:mx6ul:use-mainline-bsp  = "imx-generic-bsp:imx-mainline-bsp:mx6-generic-bsp:mx6-mainline-bsp:mx6ul-generic-bsp:mx6ul-mainline-bsp"
MACHINEOVERRIDES_EXTENDER:mx8mm:use-mainline-bsp  = "imx-generic-bsp:imx-mainline-bsp:mx8-generic-bsp:mx8-mainline-bsp:mx8m-generic-bsp:mx8m-mainline-bsp:mx8mm-generic-bsp:mx8mm-mainline-bsp"
MACHINEOVERRIDES_EXTENDER:mx8mn:use-mainline-bsp  = "imx-generic-bsp:imx-mainline-bsp:mx8-generic-bsp:mx8-mainline-bsp:mx8m-generic-bsp:mx8m-mainline-bsp:mx8mn-generic-bsp:mx8mn-mainline-bsp"
MACHINEOVERRIDES_EXTENDER:mx8mp:use-mainline-bsp  = "imx-generic-bsp:imx-mainline-bsp:mx8-generic-bsp:mx8-mainline-bsp:mx8m-generic-bsp:mx8m-mainline-bsp:mx8mp-generic-bsp:mx8mp-mainline-bsp"
MACHINEOVERRIDES_EXTENDER:mx8qxp:use-mainline-bsp = "imx-generic-bsp:imx-mainline-bsp:mx8-generic-bsp:mx8-mainline-bsp:mx8x-generic-bsp:mx8x-mainline-bsp:mx8qxp-generic-bsp:mx8qxp-mainline-bsp"
MACHINEOVERRIDES_EXTENDER:mx8dx:use-mainline-bsp  = "imx-generic-bsp:imx-mainline-bsp:mx8-generic-bsp:mx8-mainline-bsp:mx8x-generic-bsp:mx8x-mainline-bsp:mx8dx-generic-bsp:mx8dx-mainline-bsp"
MACHINEOVERRIDES_EXTENDER:mx91:use-mainline-bsp  = "imx-generic-bsp:imx-mainline-bsp:mx9-generic-bsp:mx9-mainline-bsp:mx91-generic-bsp:mx91-mainline-bsp"
MACHINEOVERRIDES_EXTENDER:mx93:use-mainline-bsp  = "imx-generic-bsp:imx-mainline-bsp:mx9-generic-bsp:mx9-mainline-bsp:mx93-generic-bsp:mx93-mainline-bsp"

MACHINEOVERRIDES_EXTENDER_FILTER_OUT = " \
    mx6 \
    mx6q \
    mx6dl \
    mx6sx \
    mx6sl \
    mx6sll \
    mx6ul \
    mx6ull \
    mx6ulz \
    \
    mx7 \
    mx7d \
    mx7ulp \
    \
    vf \
    \
    mx8 \
    mx8m \
    mx8qm \
    mx8mm \
    mx8mn \
    mx8mp \
    mx8mq \
    mx8qxp \
    mx8dx \
    mx8dxl \
    mx91 \
    mx93 \
"

MACHINEOVERRIDES_FILTERED_OUT_QA_ERROR = "%s overrides cannot be used and need conversion to use the new BSP-specific overrides. Check 'meta-freescale/scripts/convert-bsp-specific-overrides'."

# Sub-architecture support
MACHINE_SOCARCH_SUFFIX ?= ""
MACHINE_SOCARCH_SUFFIX:mx6q-nxp-bsp = "-mx6qdl"
MACHINE_SOCARCH_SUFFIX:mx6dl-nxp-bsp = "-mx6qdl"
MACHINE_SOCARCH_SUFFIX:mx6ul-nxp-bsp  = "-mx6ul"
MACHINE_SOCARCH_SUFFIX:mx8mm-nxp-bsp  = "-mx8mm"
MACHINE_SOCARCH_SUFFIX:mx8mn-nxp-bsp  = "-mx8mn"
MACHINE_SOCARCH_SUFFIX:mx8mp-nxp-bsp  = "-mx8mp"
MACHINE_SOCARCH_SUFFIX:mx8qxp-nxp-bsp = "-mx8"
MACHINE_SOCARCH_SUFFIX:mx8dx-nxp-bsp  = "-mx8"
MACHINE_SOCARCH_SUFFIX:mx91-nxp-bsp = "-mx91"
MACHINE_SOCARCH_SUFFIX:mx93-nxp-bsp = "-mx93"

# For Mainline we use a single SoC suffix as we don't have different build options
MACHINE_SOCARCH_SUFFIX:imx-mainline-bsp = "-imx"

MACHINE_ARCH_FILTER = "virtual/kernel"
MACHINE_SOCARCH_FILTER:append:imx-nxp-bsp = " \
    alsa-lib \
    gstreamer1.0 \
    weston \
"
MACHINE_SOCARCH_FILTER:append:imxvpu = " \
    imx-codec \
    imx-vpuwrap \
    libimxvpuapi \
    virtual/imxvpu \
"
MACHINE_SOCARCH_FILTER:append:imxvulkan = " \
    vulkan-headers \
    vulkan-loader \
    vulkan-tools \
    vulkan-validation-layers \
"
MACHINE_SOCARCH_FILTER:append:imxgpu = " \
    virtual/egl \
    virtual/libopenvg \
    virtual/mesa \
    virtual/opencl-icd \
    libdrm \
    cairo \
    libgal-imx \
    opencl-icd-loader \
    opencl-clhpp \
    opencl-headers \
    opencv \
    pango \
"
MACHINE_SOCARCH_FILTER:append:imxgpu2d = " \
    virtual/libg2d \
"
MACHINE_SOCARCH_FILTER:append:imxgpu3d = " \
    virtual/libgl \
    virtual/libgles1 \
    virtual/libgles2 \
"
MACHINE_SOCARCH_FILTER:append:imx-mainline-bsp = " \
    virtual/egl \
    virtual/libopenvg \
    virtual/libg2d \
    virtual/libgl \
    virtual/libgles1 \
    virtual/libgles2 \
    virtual/mesa \
    cairo \
    pango \
    qtbase \
"
MACHINE_SOCARCH_FILTER:append:mx8qm-nxp-bsp = " \
    virtual/libopenvx \
"

INHERIT += "fsl-dynamic-packagearch"

SIGGEN_EXCLUDE_SAFE_RECIPE_DEPS:append = " \
    imx-gpu-viv->kernel-module-imx-gpu-viv \
    libimxvpuapi->virtual/imxvpu \
    imx-vpuwrap->virtual/imxvpu \
    imx-codec->virtual/imxvpu \
    imx-test->virtual/imxvpu \
"

# Firmware used for boot.
IMX_EXTRA_FIRMWARE                   ?= ""
IMX_EXTRA_FIRMWARE:mx8-generic-bsp    = "imx-boot-firmware-files imx-sc-firmware imx-seco"
IMX_EXTRA_FIRMWARE:mx8m-generic-bsp   = "imx-boot-firmware-files"
IMX_EXTRA_FIRMWARE:mx8x-generic-bsp   = "imx-sc-firmware imx-seco"
IMX_EXTRA_FIRMWARE:mx91-generic-bsp   = "imx-boot-firmware-files firmware-ele-imx"
IMX_EXTRA_FIRMWARE:mx93-generic-bsp   = "imx-boot-firmware-files firmware-ele-imx"

# Firmware
MACHINE_FIRMWARE ?= ""
MACHINE_FIRMWARE:append:mx6-generic-bsp      = " firmware-imx-sdma-imx6q"
MACHINE_FIRMWARE:append:mx6q-generic-bsp     = " firmware-imx-vpu-imx6q"
MACHINE_FIRMWARE:append:mx6dl-generic-bsp    = " firmware-imx-vpu-imx6d firmware-imx-epdc"
MACHINE_FIRMWARE:append:mx8mm-generic-bsp    = " firmware-imx-sdma-imx7d"
MACHINE_FIRMWARE:append:mx8mn-generic-bsp    = " firmware-imx-sdma-imx7d firmware-imx-easrc-imx8mn"
MACHINE_FIRMWARE:append:mx8qxp-generic-bsp   = " firmware-imx-sdma-imx7d firmware-imx-vpu-amphion"
MACHINE_FIRMWARE:append:mx8dx-generic-bsp    = " firmware-imx-sdma-imx7d firmware-imx-vpu-amphion"

MACHINE_EXTRA_RRECOMMENDS += "${MACHINE_FIRMWARE}"

# Extra audio support
IMX_ALSA_EXTRA = ""
IMX_ALSA_EXTRA:imx-nxp-bsp = "imx-alsa-plugins"
MACHINE_EXTRA_RRECOMMENDS:append:mx6-nxp-bsp = " ${@bb.utils.contains('DISTRO_FEATURES', 'alsa', '${IMX_ALSA_EXTRA}', '', d)}"
MACHINE_EXTRA_RRECOMMENDS:append:mx8-nxp-bsp = " ${@bb.utils.contains('DISTRO_FEATURES', 'alsa', '${IMX_ALSA_EXTRA}', '', d)}"
MACHINE_EXTRA_RRECOMMENDS:append:mx9-nxp-bsp = " ${@bb.utils.contains('DISTRO_FEATURES', 'alsa', '${IMX_ALSA_EXTRA}', '', d)}"

# ELE extra Firmware
SECOEXT_FIRMWARE_NAME ?= "UNDEFINED"

# GStreamer 1.0 plugins
MACHINE_GSTREAMER_1_0_PLUGIN               ?= ""
MACHINE_GSTREAMER_1_0_PLUGIN:mx6dl-nxp-bsp ?= "imx-gst1.0-plugin"
MACHINE_GSTREAMER_1_0_PLUGIN:mx6q-nxp-bsp  ?= "imx-gst1.0-plugin"
MACHINE_GSTREAMER_1_0_PLUGIN:mx6ul-nxp-bsp ?= "imx-gst1.0-plugin"
MACHINE_GSTREAMER_1_0_PLUGIN:mx8-nxp-bsp   ?= "imx-gst1.0-plugin"
MACHINE_GSTREAMER_1_0_PLUGIN:mx9-nxp-bsp   ?= "imx-gst1.0-plugin"

#
# In 'meta-freescale' these gstreamer preferred versions are restricted to
# 'mx8|9-nxp-bsp' but in DEY we use them also in the 6UL, so relax the override
# to 'imx-nxp-bsp'
#
# GStreamer forked recipes
PREFERRED_VERSION_gstreamer1.0:imx-nxp-bsp              ??= "1.24.7.imx"
PREFERRED_VERSION_gstreamer1.0-plugins-bad:imx-nxp-bsp  ??= "1.24.7.imx"
PREFERRED_VERSION_gstreamer1.0-plugins-base:imx-nxp-bsp ??= "1.24.7.imx"
PREFERRED_VERSION_gstreamer1.0-plugins-good:imx-nxp-bsp ??= "1.24.7.imx"

# GStreamer copied recipes
PREFERRED_VERSION_gst-devtools:imx-nxp-bsp              ??= "1.22.5.imx"
PREFERRED_VERSION_gstreamer1.0-libav:imx-nxp-bsp        ??= "1.24.0.imx"
PREFERRED_VERSION_gstreamer1.0-plugins-ugly:imx-nxp-bsp ??= "1.24.0.imx"
PREFERRED_VERSION_gstreamer1.0-python:imx-nxp-bsp       ??= "1.24.0.imx"
PREFERRED_VERSION_gstreamer1.0-rtsp-server:imx-nxp-bsp  ??= "1.24.0.imx"
PREFERRED_VERSION_gstreamer1.0-vaapi:imx-nxp-bsp        ??= "1.24.0.imx"

# GStreamer downgrade ffmpeg
PREFERRED_VERSION_ffmpeg:imx-nxp-bsp ??= "4.4.1"

# Determines if the SoC has support for Vivante kernel driver
SOC_HAS_VIVANTE_KERNEL_DRIVER_SUPPORT        = "0"
SOC_HAS_VIVANTE_KERNEL_DRIVER_SUPPORT:imxgpu = "1"

# Handle Vivante kernel driver setting:
#   0 - machine does not have Vivante GPU driver support
#   1 - machine has Vivante GPU driver support
MACHINE_HAS_VIVANTE_KERNEL_DRIVER_SUPPORT ?= "${SOC_HAS_VIVANTE_KERNEL_DRIVER_SUPPORT}"

# Graphics libraries
PREFERRED_PROVIDER_virtual/egl      ?= "mesa"
PREFERRED_PROVIDER_virtual/libgl    ?= "mesa"
PREFERRED_PROVIDER_virtual/libgles1 ?= "mesa"
PREFERRED_PROVIDER_virtual/libgles2 ?= "mesa"

PREFERRED_PROVIDER_virtual/egl:imxgpu        ?= "imx-gpu-viv"
PREFERRED_PROVIDER_virtual/libgl:imxgpu      ?= "mesa-gl"
PREFERRED_PROVIDER_virtual/mesa:imxgpu       ?= "mesa-gl"
PREFERRED_PROVIDER_virtual/libgles1:imxgpu3d ?= "imx-gpu-viv"
PREFERRED_PROVIDER_virtual/libgles2:imxgpu3d ?= "imx-gpu-viv"
PREFERRED_PROVIDER_virtual/libgles3:imxgpu3d ?= "imx-gpu-viv"
PREFERRED_PROVIDER_virtual/libg2d            ?= "imx-gpu-g2d"
PREFERRED_PROVIDER_virtual/libg2d:imxdpu     ?= "imx-dpu-g2d"
PREFERRED_PROVIDER_virtual/libg2d:mx93-nxp-bsp ?= "imx-pxp-g2d"
PREFERRED_PROVIDER_opencl-clhpp:imxgpu       ?= "imx-gpu-viv"
PREFERRED_PROVIDER_opencl-headers:imxgpu     ?= "imx-gpu-viv"
PREFERRED_PROVIDER_opencl-icd-loader:imxgpu  ?= "imx-gpu-viv"
PREFERRED_PROVIDER_virtual/opencl-icd:imxgpu ?= "imx-gpu-viv"

PREFERRED_VERSION_weston:imx-nxp-bsp      ??= "12.0.4.imx"
# i.MX 6 & 7 stay on weston 10.0 for fbdev
PREFERRED_VERSION_weston:mx6-nxp-bsp      ??= "10.0.5.imx"
PREFERRED_VERSION_weston:mx7-nxp-bsp      ??= "10.0.5.imx"
PREFERRED_VERSION_weston:imx-mainline-bsp   = ""

PREFERRED_VERSION_wayland-protocols:imx-nxp-bsp  ??= "1.32.imx"

# Use i.MX libdrm Version
PREFERRED_VERSION_libdrm:imx-nxp-bsp ??= "2.4.116.imx"

PREFERRED_VERSION_spirv-headers:imxvulkan            ??= "1.3.275.0.imx"
PREFERRED_VERSION_spirv-tools:imxvulkan              ??= "1.3.275.0.imx"
PREFERRED_VERSION_vulkan-headers:imxvulkan           ??= "1.3.275.0.imx"
PREFERRED_VERSION_vulkan-loader:imxvulkan            ??= "1.3.275.0.imx"
PREFERRED_VERSION_vulkan-tools:imxvulkan             ??= "1.3.275.0.imx"
PREFERRED_VERSION_vulkan-validation-layers:imxvulkan ??= "1.3.275.0.imx"

# Use i.MX optee Version
PREFERRED_VERSION_optee-os:mx8-nxp-bsp     ??= "4.4.0.imx"
PREFERRED_VERSION_optee-os:mx9-nxp-bsp     ??= "4.4.0.imx"
PREFERRED_VERSION_optee-client:mx8-nxp-bsp ??= "4.4.0.imx"
PREFERRED_VERSION_optee-client:mx9-nxp-bsp ??= "4.4.0.imx"
PREFERRED_VERSION_optee-test:mx8-nxp-bsp   ??= "4.4.0.imx"
PREFERRED_VERSION_optee-test:mx9-nxp-bsp   ??= "4.4.0.imx"

# Optee runtime packages to install
OPTEE_PKGS ??= "optee-client optee-os"

# Use i.MX opencv Version
PREFERRED_VERSION_opencv:mx8-nxp-bsp ??= "4.10.0.imx"
PREFERRED_VERSION_opencv:mx9-nxp-bsp ??= "4.10.0.imx"

EXTRA_IMAGEDEPENDS += "u-boot"

KERNEL_IMAGETYPE = "zImage"
KERNEL_IMAGETYPE:aarch64 = "Image.gz"

MACHINE_FEATURES = "usbgadget usbhost vfat alsa touchscreen screen"

# IMX specific udev extra rules added when device manager is systemd/udev
UDEV_EXTRA_RULES:append = " udev-rules-imx"

# U-Boot 2017.X requires a specific dtc version.
PREFERRED_VERSION_dtc-native:ccimx6 ?= "1.4.5"
PREFERRED_VERSION_nativesdk-dtc:ccimx6 ?= "1.4.5"

DEY_SOC_VENDOR = "NXP"
