#@TYPE: Machine
#@NAME: ConnectCore for MX51 module.
#@DESCRIPTION: Machine configuration for Digi's ConnectCore for MX51 module.

include conf/machine/include/imx-digi-base.inc
include conf/machine/include/tune-cortexa8.inc

DIGI_FAMILY = "ccimx5:ccimx51"
SOC_FAMILY  = "mx5:mx51:${DIGI_FAMILY}"

# Disable perf features when using kernel 2.6.35 as perf fails to build otherwise
PERF_FEATURES_ENABLE = ""

# Platform u-boot settings
UBOOT_ENTRYPOINT = "0x90008000"
UBOOT_SUFFIX = "bin"

# Firmware
MACHINE_FIRMWARE = "firmware-imx-vpu-imx51 firmware-imx-sdma-imx51"

MACHINE_EXTRA_RDEPENDS    += "mtd-utils-ubifs nvram ubootenv update-flash"
MACHINE_EXTRA_RRECOMMENDS += "${MACHINE_FIRMWARE}"
MACHINE_EXTRA_RRECOMMENDS += "kernel-module-mma7455l"
MACHINE_EXTRA_RRECOMMENDS += "kernel-module-smsc911x"
MACHINE_EXTRA_RRECOMMENDS += "kernel-module-scc2-driver"
MACHINE_EXTRA_RRECOMMENDS += "kernel-module-sahara"
MACHINE_EXTRA_RRECOMMENDS += "kernel-module-mt9v111-camera"
MACHINE_EXTRA_RRECOMMENDS += "kernel-module-pmic-battery"

KERNEL_MODULE_AUTOLOAD += "mma7455l"
KERNEL_MODULE_AUTOLOAD += "smsc911x"
KERNEL_MODULE_AUTOLOAD += "scc2_driver"
KERNEL_MODULE_AUTOLOAD += "sahara"
KERNEL_MODULE_AUTOLOAD += "mt9v111_camera"
KERNEL_MODULE_AUTOLOAD += "pmic_battery"

#
# Supported variants
#
# Maintain in sync with the same table in platform local.conf template.
#
# Name          WiFi    Eth2    RAM=128MB    NAND=2GB     Accel   GPU     VPU
# ---------------------------------------------------------------------------
# 128           N       N           Y           N         N       N       N
# 128a          N       N           Y           N         Y       N       N
# 128agv        N       N           Y           N         Y       Y       Y
# agv           N       N           N           N         Y       Y       Y
# agv4k         N       N           N           Y         Y       Y       Y
# eagv          N       Y           N           N         Y       Y       Y
# eagv4k        N       Y           N           Y         Y       Y       Y
# w             Y       N           N           N         N       N       N
# w128a         Y       N           Y           N         Y       N       N
# w128agv       Y       N           Y           N         Y       Y       Y
# wagv          Y       N           N           N         Y       Y       Y
# weagv         Y       Y           N           N         Y       Y       Y

# Per-variant machine features
MACHINE_FEATURES_append_128a = " accelerometer"
MACHINE_FEATURES_append_128agv = " accelerometer accel-video accel-graphics"
MACHINE_FEATURES_append_agv = " accelerometer accel-video accel-graphics"
MACHINE_FEATURES_append_agv4k = " accelerometer accel-video accel-graphics"
MACHINE_FEATURES_append_eagv = " ext-eth accelerometer accel-video accel-graphics"
MACHINE_FEATURES_append_eagv4k = " ext-eth accelerometer accel-video accel-graphics"
MACHINE_FEATURES_append_w = " wifi"
MACHINE_FEATURES_append_w128a = " wifi accelerometer"
MACHINE_FEATURES_append_w128agv = " wifi accelerometer accel-video accel-graphics"
MACHINE_FEATURES_append_wagv = " wifi accelerometer accel-video accel-graphics"
MACHINE_FEATURES_append_weagv = " wifi ext-eth accelerometer accel-video accel-graphics"

# Per-variant u-boot configuration
UBOOT_CONFIG ??= "ccimx51js"
UBOOT_CONFIG_128 = "ccimx51js_128sdram"
UBOOT_CONFIG_128a = "ccimx51js_128sdram"
UBOOT_CONFIG_128agv = "ccimx51js_128sdram"
UBOOT_CONFIG_w128a = "ccimx51js_128sdram"
UBOOT_CONFIG_w128agv = "ccimx51js_128sdram"
UBOOT_CONFIG_weagv = "ccimx51js_ext_eth"
UBOOT_CONFIG[ccimx51js] = "ccimx51js_config"
UBOOT_CONFIG[ccimx51js_128sdram] = "ccimx51js_128sdram_config"
UBOOT_CONFIG[ccimx51js_ext_eth] = "ccimx51js_ext_eth_config"
