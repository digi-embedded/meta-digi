# Digi BSP default providers
PREFERRED_PROVIDER_linux-libc-headers ?= "linux-libc-headers-imx"
PREFERRED_PROVIDER_u-boot ?= "u-boot-imx"
PREFERRED_PROVIDER_virtual/kernel ?= "linux-del"
PREFERRED_PROVIDER_virtual/xserver = "xserver-xorg"

PREFERRED_PROVIDER_virtual/libx11 ?= "${@base_contains('DISTRO_FEATURES', 'x11', 'libx11', '', d)}"
PREFERRED_PROVIDER_jpeg-native ?= "jpeg-native"

PREFERRED_VERSION_udev = "173"

# Digi BSP default settings
IMAGE_CLASSES = "image_types_digi"

# Do not enable console on VT (see sysvinit-inittab recipe)
USE_VT = "0"
