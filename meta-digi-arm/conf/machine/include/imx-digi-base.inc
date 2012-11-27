# Provides the i.MX architecture common settings
include conf/machine/include/imx-base.inc

# Overrides the default fsl settings

# Single override file for settings, versions and providers.
include conf/machine/include/digi-defaults.inc

UBOOT_MAKE_TARGET = "u-boot.bin"
UBOOT_SUFFIX = "bin"

PREFERRED_PROVIDER_virtual/libgl_mx5 = "${@base_contains('DISTRO_FEATURES', 'x11', 'amd-gpu-x11-bin-mx51', 'amd-gpu-bin-mx51', d)}"

IMAGE_FSTYPES = "jffs2 tar.bz2 ext3 sdcard"
