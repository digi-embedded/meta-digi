#@TYPE: Machine
#@NAME: ConnectCore 6 module.
#@DESCRIPTION: Machine configuration for Digi's ConnectCore 6 module.

include conf/machine/include/imx-digi-base.inc
include conf/machine/include/tune-cortexa9.inc

SOC_FAMILY = "mx6:mx6q"

# Platform u-boot settings
UBOOT_ENTRYPOINT = "0x10800000"
UBOOT_SUFFIX = "imx"

# Firmware
MACHINE_FIRMWARE ?= ""
MACHINE_FIRMWARE_append = " firmware-imx-vpu-imx6q"

MACHINE_EXTRA_RRECOMMENDS += "${MACHINE_FIRMWARE}"
MACHINE_EXTRA_RRECOMMENDS += "fsl-alsa-plugins"

#
# Supported variants
#
# To build any of these variants configure MACHINE_VARIANT on the project's
# local.conf to the variant name.
#
# Name          WiFi
# ------------------
# -             N       (empty MACHINE_VARIANT="")
# w             Y

# All variants support
MACHINE_FEATURES += "accel-graphics accel-video"

# Per-variant machine features
MACHINE_FEATURES_append_w = " wifi"
