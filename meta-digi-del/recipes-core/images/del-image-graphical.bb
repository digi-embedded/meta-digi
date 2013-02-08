#
# Copyright (C) 2012 Digi International.
#
DESCRIPTION = "Graphical image based on SATO, a gnome mobile environment visual style."

IMAGE_FEATURES += "splash package-management x11-base x11-sato"

LICENSE = "MIT"

VIRTUAL-RUNTIME_dev_manager = "udev"

include del-image-minimal.bb

DISTRO_FEATURES += "pulseaudio"
WEB = "web-webkit"

SOC_IMAGE_INSTALL = ""
SOC_IMAGE_INSTALL_mx5 = "amd-gpu-x11-bin-mx51"

IMAGE_INSTALL += " \
	${SOC_IMAGE_INSTALL} \
	pointercal-xinput \
    "

export IMAGE_BASENAME = "del-image-graphical"
