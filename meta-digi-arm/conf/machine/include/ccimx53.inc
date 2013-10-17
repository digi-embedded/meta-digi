#@TYPE: Machine
#@NAME: ConnectCore for MX53 module.
#@DESCRIPTION: Machine configuration for Digi's ConnectCore for MX53 module.

include conf/machine/include/imx-digi-base.inc

SOC_FAMILY = "mx5:mx53"

UBOOT_ENTRYPOINT  = "0x70800000"
UBOOT_LOADADDRESS = "0x70800000"

# Firmware
MACHINE_FIRMWARE = "firmware-imx-vpu-imx53 firmware-imx-sdma-imx53"
MACHINE_EXTRA_RRECOMMENDS += "${MACHINE_FIRMWARE}"
MACHINE_EXTRA_RRECOMMENDS += "kernel-module-mma7455l"
MACHINE_EXTRA_RRECOMMENDS += "kernel-module-smsc911x"
MACHINE_EXTRA_RRECOMMENDS += "kernel-module-scc2-driver"
MACHINE_EXTRA_RRECOMMENDS += "kernel-module-sahara"
MACHINE_EXTRA_RRECOMMENDS += "kernel-module-mt9v111-camera"

module_autoload_mma7455l = "mma7455l"
module_autoload_smsc911x = "smsc911x"
module_autoload_scc2_driver = "scc2_driver"
module_autoload_sahara = "sahara"
module_autoload_mt9v111_camera = "mt9v111_camera"

#
# Supported variants
#
# To build any of these variants configure MACHINE_VARIANT on the project's
# local.conf to the variant name.
#
# Name          WiFi    Eth2    RAM     NAND
# ------------------------------------------
# -             N       N       512     512     (empty MACHINE_VARIANT="")
# 128           N       N       128     128
# 4k            N       N       512     1024
# e             N       Y       1024    512
# e4k           N       Y       512     1024
# w             Y       N       512     512
# w128          Y       N       128     128
# we            Y       Y       512     512

# All variants support
MACHINE_FEATURES += "accelerometer accel-graphics accel-video"

# Per-variant machine features
MACHINE_FEATURES_append_e = " ext-eth"
MACHINE_FEATURES_append_e4k = " ext-eth"
MACHINE_FEATURES_append_w = " wifi"
MACHINE_FEATURES_append_w128 = " wifi"
MACHINE_FEATURES_append_we = " wifi ext-eth"

# Per-variant u-boot configuration
UBOOT_CONFIG_VARIANT_128 = "128sdram"
UBOOT_CONFIG_VARIANT_4k = "4Kpage"
UBOOT_CONFIG_VARIANT_e = "ext_eth"
UBOOT_CONFIG_VARIANT_e4k = "4Kpage"
UBOOT_CONFIG_VARIANT_w128 = "128sdram"
UBOOT_CONFIG_VARIANT_we = "ext_eth"
