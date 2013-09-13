# Digi BSP default providers
PREFERRED_PROVIDER_linux-libc-headers ?= "linux-libc-headers-dey"
PREFERRED_PROVIDER_u-boot ?= "u-boot-dey"
PREFERRED_VERSION_u-boot-dey ?= "2009.08"
PREFERRED_PROVIDER_virtual/kernel ?= "linux-dey"
PREFERRED_VERSION_linux-dey ?= "2.6.35.14"
PREFERRED_VERSION_linux-libc-headers-dey ?= "${PREFERRED_VERSION_linux-dey}"
PREFERRED_PROVIDER_virtual/xserver = "xserver-xorg"
PREFERRED_PROVIDER_virtual/libx11 ?= "${@base_contains('DISTRO_FEATURES', 'x11', 'libx11', '', d)}"
PREFERRED_PROVIDER_jpeg-native ?= "jpeg-native"

IS_KERNEL_2X ?= "${@base_version_less_or_equal('PREFERRED_VERSION_linux-dey', '2.6.35.14', '1', '', d)}"

# Digi BSP default settings
IMAGE_CLASSES = "image_types_digi"

# Do not enable console on VT (see sysvinit-inittab recipe)
USE_VT = "0"

# Add the machine variant to the valid machine override
MACHINEOVERRIDES .= "${@['', ':${MACHINE_VARIANT}']['${MACHINE_VARIANT}' != '']}"
MACHINE_VARIANT ?= ""

# U-Boot settings
UBOOT_BSTR           ??= ""
UBOOT_CONFIG_VARIANT ??= ""
UBOOT_BINARY          ?= "u-boot${UBOOT_BSTR}.${UBOOT_SUFFIX}"
UBOOT_IMAGE           ?= 'u-boot-${MACHINE}${@base_conditional( "UBOOT_CONFIG_VARIANT", "", "", "_${UBOOT_CONFIG_VARIANT}", d )}${UBOOT_BSTR}-${PV}-${PR}.${UBOOT_SUFFIX}'
UBOOT_MACHINE         ?= '${MACHINE}${@base_conditional( "UBOOT_CONFIG_VARIANT", "", "", "_${UBOOT_CONFIG_VARIANT}", d )}_config'
UBOOT_MAKE_TARGET     ?= "${UBOOT_BINARY}"
UBOOT_SYMLINK         ?= 'u-boot-${MACHINE}${@base_conditional( "UBOOT_CONFIG_VARIANT", "", "", "_${UBOOT_CONFIG_VARIANT}", d )}${UBOOT_BSTR}.${UBOOT_SUFFIX}'
