# Provides the i.MXS architecture common settings
include conf/machine/include/mxs-base.inc

# Overrides the default FSL settings

# Single override file for settings, versions and providers.
include conf/machine/include/digi-defaults.inc

# Flash image types
SDCARD_ROOTFS ?= "${DEPLOY_DIR_IMAGE}/${IMAGE_NAME}.rootfs.ext3"

# [TODO] add SDCARD support
# At the moment it's failing due to wrong u-boot image
IMAGE_FSTYPES ?= "tar.bz2"
IMAGE_FSTYPES += "sum.jffs2 ubifs"

# Sector size 128 KiB
JFFS2_ERASEBLOCK = "0x20000"

# Use the common short option for 'mkfs.jffs2' and 'sumtool'
JFFS2_ENDIANNESS = "-l"
EXTRA_IMAGECMD_sum.jffs2 = "${EXTRA_IMAGECMD_jffs2}"

# mkfs.ubifs parameters
MKUBIFS_ARGS = "-m 2048 -e 126976 -c 2047"
