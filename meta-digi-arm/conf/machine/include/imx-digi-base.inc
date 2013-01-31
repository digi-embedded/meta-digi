# Provides the i.MXC architecture common settings
include conf/machine/include/imx-base.inc

# Overrides the default FSL settings

# Single override file for settings, versions and providers.
include conf/machine/include/digi-defaults.inc

UBOOT_MAKE_TARGET = "u-boot.bin"
UBOOT_SUFFIX = "bin"
UBOOT_PADDING = "2"

# Set UBOOT_CONFIG_VARIANT in local.conf accordingly, default is empty:
# MX5X: 128sdram,ext_eth,128sdram_test,128sdram_dbg,test,dbg,test_dbg,db_pp,db_pa,db_rp,db_ra
# MX51: EAK
# MX53: 4Kpage

UBOOT_CONFIG_VARIANT ?= ""

UBOOT_MACHINE = '${MACHINE}${@base_conditional( "UBOOT_CONFIG_VARIANT", "", "_", "_${UBOOT_CONFIG_VARIANT}_", d )}config'
UBOOT_IMAGE = 'u-boot-${MACHINE}${@base_conditional( "UBOOT_CONFIG_VARIANT", "", "-", "-${UBOOT_CONFIG_VARIANT}-", d )}${PV}-${PR}.${UBOOT_SUFFIX}'
UBOOT_SYMLINK = 'u-boot-${MACHINE}${@base_conditional( "UBOOT_CONFIG_VARIANT", "", "", "-${UBOOT_CONFIG_VARIANT}", d )}.${UBOOT_SUFFIX}'

# Flash image types
IMAGE_FSTYPES += "sum.jffs2 ubifs"
