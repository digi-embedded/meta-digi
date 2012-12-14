# Provides the i.MX architecture common settings
include conf/machine/include/imx-base.inc

# Overrides the default fsl settings

# Single override file for settings, versions and providers.
include conf/machine/include/digi-defaults.inc

UBOOT_MAKE_TARGET = "u-boot.bin"
UBOOT_SUFFIX = "bin"

IMAGE_FSTYPES = "jffs2 tar.bz2 ext3 sdcard"
