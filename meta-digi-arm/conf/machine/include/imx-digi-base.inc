# Provides the i.MXC architecture common settings
include conf/machine/include/imx-base.inc

# Overrides the default FSL settings

# Single override file for settings, versions and providers.
include conf/machine/include/digi-defaults.inc

UBOOT_MAKE_TARGET = "u-boot.bin"
UBOOT_SUFFIX = "bin"
UBOOT_PADDING = "2"

# Flash image types
IMAGE_FSTYPES += "sum.jffs2 ubifs"

PREFERRED_VERSION_wpa-supplicant = "1.0"
