#
# Copyright (C) 2016-2022 Digi International.
#

IMAGE_FEATURES += " \
    dey-network \
    eclipse-debug \
    package-management \
    ssh-server-dropbear \
    ${@bb.utils.contains('MACHINE_FEATURES', 'accel-video', 'dey-gstreamer', '', d)} \
    ${@bb.utils.contains('MACHINE_FEATURES', 'alsa', 'dey-audio', '', d)} \
    ${@bb.utils.contains('MACHINE_FEATURES', 'bluetooth', 'dey-bluetooth', '', d)} \
    ${@bb.utils.contains('MACHINE_FEATURES', 'wifi', 'dey-wireless', '', d)} \
"

# Remove graphical packages for non-graphical platforms
IMAGE_FEATURES:remove = "${@oe.utils.conditional('IS_HEADLESS', 'true', ' splash ', '', d)}"

CORE_IMAGE_BASE_INSTALL += "dey-examples-digiapix"

# The connectcore demo was removed from 'packagegroup-dey-core' for the
# 6UL (because of rootfs space limits). Add it here, to install it in the
# non-graphical core-image-base.
CORE_IMAGE_BASE_INSTALL:append:ccimx6ul = " connectcore-demo-example"

# SDK features (for toolchains generated from an image with populate_sdk)
SDKIMAGE_FEATURES ?= "dev-pkgs dbg-pkgs staticdev-pkgs"

# Add our dey-image tweaks to the final image (like /etc/build info)
inherit dey-image

# Do not install udev-cache
BAD_RECOMMENDATIONS += "udev-cache"
