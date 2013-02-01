#
# Copyright (C) 2012 Digi International.
#
DESCRIPTION = "Graphical image based on SATO, a gnome mobile environment visual style."

IMAGE_FEATURES += "splash package-management x11-base x11-sato"

LICENSE = "MIT"

VIRTUAL-RUNTIME_dev_manager = "udev"

include del-image-minimal.bb

IMAGE_INSTALL += "packagegroup-core-x11-sato-games"

#IMAGE_FEATURES += "debug-tweaks"
DISTRO_FEATURES += "pulseaudio"
WEB = "web-webkit"

SOC_EXTRA_IMAGE_FEATURES ?= "tools-testapps"

# mesa-demos is currently broken when building with other GL library
# so we avoid it by now and tools-testapps includes it.
SOC_EXTRA_IMAGE_FEATURES_mx5 = ""
SOC_EXTRA_IMAGE_FEATURES_mx6 = ""

# Add extra image features
EXTRA_IMAGE_FEATURES += " \
    ${SOC_EXTRA_IMAGE_FEATURES} \
    nfs-server \
    tools-debug \
    tools-profile \
    qt4-pkgs \
"

SOC_IMAGE_INSTALL = ""
SOC_IMAGE_INSTALL_mx5 = "glcubes-demo"

# TODO: test the following
# packagegroup-fsl-tools-testapps \
# packagegroup-fsl-tools-benchmark \
# packagegroup-qt-in-use-demos \

IMAGE_INSTALL += " \
    ${SOC_IMAGE_INSTALL} \
    cpufrequtils \
    nano \
    qt4-plugin-phonon-backend-gstreamer \
    qt4-demos \
    qt4-examples \
    fsl-gui-extrafiles \
    "

export IMAGE_BASENAME = "del-image-graphical"
