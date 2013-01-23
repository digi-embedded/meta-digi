# Digi BSP default providers
PREFERRED_PROVIDER_virtual/kernel = "linux-imx"
PREFERRED_PROVIDER_u-boot = "u-boot-imx"

PREFERRED_PROVIDER_virtual/egl_mx5      = "${@base_contains('DISTRO_FEATURES', 'x11', 'amd-gpu-x11-bin-mx51', 'amd-gpu-bin-mx51', d)}"
PREFERRED_PROVIDER_virtual/libgles1_mx5 = "${@base_contains('DISTRO_FEATURES', 'x11', 'amd-gpu-x11-bin-mx51', 'amd-gpu-bin-mx51', d)}"
PREFERRED_PROVIDER_virtual/libgles2_mx5 = "${@base_contains('DISTRO_FEATURES', 'x11', 'amd-gpu-x11-bin-mx51', 'amd-gpu-bin-mx51', d)}"

# Digi BSP default settings
IMAGE_CLASSES += "image_types_digi"

# Do not enable console on VT (see sysvinit-inittab recipe)
USE_VT = "0"
