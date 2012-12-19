# Provides the i.MXC architecture common settings
include conf/machine/include/imx-base.inc

# Overrides the default FSL settings

# Single override file for settings, versions and providers.
include conf/machine/include/digi-defaults.inc

UBOOT_MAKE_TARGET = "u-boot.bin"
UBOOT_SUFFIX = "bin"

# Flash image types
IMAGE_FSTYPES += "sum.jffs2 ubifs"

# Sector size 128 KiB
JFFS2_ERASEBLOCK = "0x20000"

# Use the common short option for 'mkfs.jffs2' and 'sumtool'
JFFS2_ENDIANNESS = "-l"
EXTRA_IMAGECMD_sum.jffs2 = "${EXTRA_IMAGECMD_jffs2}"

# mkfs.ubifs parameters
MKUBIFS_ARGS = "-m 2048 -e 129024 -c 4095"
