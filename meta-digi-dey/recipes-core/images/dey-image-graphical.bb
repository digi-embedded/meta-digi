#
# Copyright (C) 2012 Digi International.
#
DESCRIPTION = "Graphical image based on SATO, a gnome mobile environment visual style."

PR = "${INC_PR}.0"

IMAGE_FEATURES += "splash package-management dey-gstreamer x11-base x11-sato"

LICENSE = "MIT"

include dey-image-minimal.bb

DISTRO_FEATURES += "pulseaudio"
WEB = "web-webkit"

SOC_IMAGE_INSTALL = ""
SOC_IMAGE_INSTALL_mx5 = "amd-gpu-x11-bin-mx51"

IMAGE_INSTALL += " \
	${SOC_IMAGE_INSTALL} \
	pointercal-xinput \
    "
