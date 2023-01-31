#
# Copyright (C) 2016-2020 Digi International.
#
DESCRIPTION = "DEY image with graphical libraries"
LICENSE = "MIT"

GRAPHICAL_CORE ??= "qt"

SOC_PACKAGES = ""
SOC_PACKAGES:ccimx6 = "imx-gpu-viv-demos imx-gpu-viv-tools"

IMAGE_INSTALL = " \
    packagegroup-dey-core \
    ${CORE_IMAGE_EXTRA_INSTALL} \
    ${SOC_PACKAGES} \
"

IMAGE_FEATURES += " \
    dey-network \
    dey-${GRAPHICAL_CORE} \
    eclipse-debug \
    package-management \
    ssh-server-dropbear \
    ${@bb.utils.contains('DISTRO_FEATURES', 'wayland', 'weston', \
       bb.utils.contains('DISTRO_FEATURES',     'x11', 'x11-base x11-sato', \
                                                       '', d), d)} \
    ${@bb.utils.contains('MACHINE_FEATURES', 'accel-video', 'dey-gstreamer', '', d)} \
    ${@bb.utils.contains('MACHINE_FEATURES', 'alsa', 'dey-audio', '', d)} \
    ${@bb.utils.contains('MACHINE_FEATURES', 'bluetooth', 'dey-bluetooth', '', d)} \
    ${@bb.utils.contains('MACHINE_FEATURES', 'wifi', 'dey-wireless', '', d)} \
"

# Separately add 'dey-gstreamer' to ccimx6ulsbc platform.
# This platform does not have 'accel-video' but we want gstreamer for using
# it with video/camera
IMAGE_FEATURES:append:ccimx6ulsbc = " dey-gstreamer"

IMAGE_FEATURES:append:ccmp15 = " ${@bb.utils.contains('DISTRO_FEATURES', 'gstreamer', 'dey-gstreamer', '', d)}"

# Remove some features to reduce the rootfs size to fit in the internal storage.
IMAGE_FEATURES:remove:ccimx6ul = "package-management"

# SDK features (for toolchains generated from an image with populate_sdk)
SDKIMAGE_FEATURES ?= "dev-pkgs dbg-pkgs staticdev-pkgs"

IMAGE_LINGUAS = ""

inherit core-image
inherit dey-image
inherit features_check

CONFLICT_DISTRO_FEATURES = "directfb"

IMAGE_ROOTFS_SIZE = "8192"

# Do not install udev-cache
BAD_RECOMMENDATIONS += "udev-cache"
BAD_RECOMMENDATIONS += "eudev-hwdb"

export IMAGE_BASENAME = "dey-image-${GRAPHICAL_CORE}-${GRAPHICAL_BACKEND}"

CORE_IMAGE_EXTRA_INSTALL += " \
    ${@bb.utils.contains('DISTRO_FEATURES', 'x11 wayland', 'weston-xwayland', '', d)} \
    dey-examples-digiapix \
"
