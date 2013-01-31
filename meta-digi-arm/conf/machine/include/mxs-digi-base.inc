# Provides the i.MXS architecture common settings
include conf/machine/include/mxs-base.inc

# Overrides the default FSL settings

# Single override file for settings, versions and providers.
include conf/machine/include/digi-defaults.inc

# Flash image types
SDCARD_ROOTFS ?= "${DEPLOY_DIR_IMAGE}/${IMAGE_NAME}.rootfs.ext3"

EXTRA_IMAGEDEPENDS = "u-boot"

# [TODO] add SDCARD support
# At the moment it's failing due to wrong u-boot image
IMAGE_FSTYPES ?= "tar.bz2"
IMAGE_FSTYPES += "sum.jffs2 ubifs"

# wpa_supplicant driver to use in '/etc/network/interfaces'
WPA_DRIVER = "nl80211"
