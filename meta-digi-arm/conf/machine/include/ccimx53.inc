#@TYPE: Machine
#@NAME: ConnectCore for MX53 module.
#@DESCRIPTION: Machine configuration for Digi's ConnectCore for MX53 module.

include conf/machine/include/imx-digi-base.inc

SOC_FAMILY = "mx53:mx5"

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

# Supported variants

# Name		WiFi	Eth2	RAM	NAND
# ----		---	---	---	----
# we		Y	Y	512	512
# w		Y	N	512	512
# w128		Y	N	128	128
# -		N	N	512	512
# e		N	Y	1024	512
# 4k		N	N	512	1024
# e		N	Y	512	1024
# 128		N	N	128	128

# All variants support
MACHINE_FEATURES += "accel-video"
MACHINE_FEATURES += "accel-graphics"
MACHINE_FEATURES += "accelerometer"

# Variant configuration

MACHINE_FEATURES_append_we = " wifi ext-eth"
UBOOT_CONFIG_VARIANT_append_we = " ext_eth"
MACHINE_FEATURES_append_w = " wifi"
MACHINE_FEATURES_append_w128 = " wifi"
UBOOT_CONFIG_VARIANT_w128 = "128sdram"
MACHINE_FEATURES_append_e = " ext-ext"
UBOOT_CONFIG_VARIANT_e = "ext_eth"
UBOOT_CONFIG_VARIANT_4k = "4Kpage"
MACHINE_FEATURES_append_e = " ext-eth"
UBOOT_CONFIG_VARIANT_e = "ext_eth"
UBOOT_CONFIG_VARIANT_128 = "128sdram"

