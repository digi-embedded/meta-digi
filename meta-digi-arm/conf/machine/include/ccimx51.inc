#@TYPE: Machine
#@NAME: ConnectCore for MX51 module.
#@DESCRIPTION: Machine configuration for Digi's ConnectCore for MX51 module.

include conf/machine/include/imx-digi-base.inc

SOC_FAMILY = "mx51:mx5"

UBOOT_ENTRYPOINT  = "0x94000000"
UBOOT_LOADADDRESS = "0x94000000"

# Firmware
MACHINE_FIRMWARE = "firmware-imx-vpu-imx51 firmware-imx-sdma-imx51"

MACHINE_EXTRA_RRECOMMENDS += "${MACHINE_FIRMWARE}"
MACHINE_EXTRA_RRECOMMENDS += "kernel-module-mma7455l"
MACHINE_EXTRA_RRECOMMENDS += "kernel-module-smsc911x"
MACHINE_EXTRA_RRECOMMENDS += "kernel-module-scc2-driver"
MACHINE_EXTRA_RRECOMMENDS += "kernel-module-sahara"
MACHINE_EXTRA_RRECOMMENDS += "kernel-module-mt9v111-camera"
MACHINE_EXTRA_RRECOMMENDS += "kernel-module-pmic-battery"

module_autoload_mma7455l = "mma7455l"
module_autoload_smsc911x = "smsc911x"
module_autoload_scc2_driver = "scc2_driver"
module_autoload_sahara = "sahara"
module_autoload_mt9v111_camera = "mt9v111_camera"
module_autoload_pmic_battery = "pmic_battery"

# Supported variants

# Name		WiFi	Eth2	RAM	Accel	GPU	VPU
# ----		---	---	---	----	---	---
# wagv		Y	N	256	Y	Y	Y
# weagv		Y	Y	512	Y	Y	Y
# w128agv	Y	N	128	Y	Y	Y
# eavg		N	Y	512	Y	Y	Y
# agv		N	N	256	Y	Y	Y
# w		Y	N	512	N	N	N
# 128		N	N	128	N	N	N
# 128avg	N	N	128	Y	Y	Y
# 128a		N	N	128	Y	N	N
# w128a		Y	N	128	Y	N	N


MACHINE_FEATURES_append_wagv = " wifi accelerometer accel-video accel-graphics"
MACHINE_FEATURES_append_weagv = " wifi ext-eth accelerometer accel-video accel-graphics"
UBOOT_CONFIG_VARIANT_weagv = "ext_eth"
MACHINE_FEATURES_append_w128agv = " wifi accelerometer accel-video accel-graphics"
UBOOT_CONFIG_VARIANT_w128agv = "128sdram"
MACHINE_FEATURES_append_eavg = " ext-eth accelerometer accel-video accel-graphics"
MACHINE_FEATURES_append_agv = " accelerometer accel-video accel-graphics"
MACHINE_FEATURES_append_w = " wifi"
UBOOT_CONFIG_VARIANT_128 = "128sdram"
MACHINE_FEATURES_append_128avg = " accelerometer accel-video accel-graphics"
UBOOT_CONFIG_VARIANT_128avg = "128sdram"
MACHINE_FEATURES_append_128a = " accelerometer"
UBOOT_CONFIG_VARIANT_128a = "128sdram"
MACHINE_FEATURES_append_w128a = " wifi accelerometer"
UBOOT_CONFIG_VARIANT_w128a = "128sdram"

