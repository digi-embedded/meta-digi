#
# Copyright (C) 2012 Digi International.
#
DESCRIPTION = "Graphical image based on SATO, a gnome mobile environment visual style."

PR = "${INC_PR}.0"

IMAGE_FEATURES += " \
    dey-qt \
    package-management \
    x11-base \
    x11-sato \
"

LICENSE = "MIT"

include dey-image-minimal.bb

REQUIRED_DISTRO_FEATURES = "x11"

WEB = "web-webkit"

SOC_IMAGE_INSTALL = ""
SOC_IMAGE_INSTALL_mx5 = "amd-gpu-x11-bin-mx51"
SOC_IMAGE_INSTALL_mx6 = "gpu-viv-bin-mx6q gpu-viv-g2d"

IMAGE_INSTALL += " \
    ${SOC_IMAGE_INSTALL} \
    ${@base_contains("MACHINE_FEATURES", "accel-video", "owl-video", "", d)} \
    pointercal-xinput \
"

# Do not install some of the 'RRECOMMENDS_qt4-demos' to save space:
# 'qt4-demos-doc' for all platforms and 'qt4-examples' for ccardimx28
BAD_RECOMMENDATIONS += "qt4-demos-doc"
BAD_RECOMMENDATIONS_append_ccardimx28 = " qt4-examples"
