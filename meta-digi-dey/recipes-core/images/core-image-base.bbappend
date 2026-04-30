#
# Copyright (C) 2016-2026, Digi International Inc.
#

IMAGE_FEATURES += " \
    dey-network \
    eclipse-debug \
    ssh-server-dropbear \
    ${@bb.utils.contains('DISTRO_FEATURES', 'gstreamer', 'dey-gstreamer', '', d)} \
    ${@bb.utils.contains('MACHINE_FEATURES', 'alsa', 'dey-audio', '', d)} \
    ${@bb.utils.contains('MACHINE_FEATURES', 'bluetooth', 'dey-bluetooth', '', d)} \
    ${@bb.utils.contains('MACHINE_FEATURES', 'wifi', 'dey-wireless', '', d)} \
"

# Remove splash from poky core-image-base
IMAGE_FEATURES:remove = "splash"

CORE_IMAGE_BASE_INSTALL += "dey-examples-digiapix"

# The connectcore demo was removed from 'packagegroup-dey-core' for the
# 6UL (because of rootfs space limits). Add it here, to install it in the
# non-graphical core-image-base.
CORE_IMAGE_BASE_INSTALL:append:ccimx6ul = " connectcore-demo-example"

# Add our dey-image tweaks to the final image (like /etc/buildinfo info)
inherit dey-image
