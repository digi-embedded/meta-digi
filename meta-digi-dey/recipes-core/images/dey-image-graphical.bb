#
# Copyright (C) 2012 Digi International.
#
DESCRIPTION = "Graphical image based on SATO, a gnome mobile environment visual style."

IMAGE_FEATURES += " \
    dey-qt \
    ${@base_contains('DISTRO_FEATURES', 'x11', 'x11-base x11-sato', '', d)} \
"

LICENSE = "MIT"

include dey-image-minimal.bb

SOC_IMAGE_INSTALL = ""
SOC_IMAGE_INSTALL_mx6 = "imx-gpu-viv-demos imx-gpu-viv-tools"

IMAGE_INSTALL += " \
    ${SOC_IMAGE_INSTALL} \
"
