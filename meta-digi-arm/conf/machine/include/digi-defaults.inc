# Digi BSP default providers

PREFERRED_PROVIDER_virtual/xserver = "xserver-xorg"
PREFERRED_PROVIDER_virtual/kernel ?= "linux-digi-2.6.35"
PREFERRED_PROVIDER_linux-libc-headers ?= "linux-libc-headers-imx"
PREFERRED_PROVIDER_u-boot ?= "u-boot-digi"

# Digi BSP default settings

IMAGE_CLASSES += "image_types_digi"

# Digi BSP default versions

PREFERRED_VERSION_udev = "173"
