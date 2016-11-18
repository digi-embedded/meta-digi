#
# Copyright (C) 2016 Digi International.
#
DESCRIPTION = "DEY image with QT graphical libraries"
LICENSE = "MIT"

SOC_PACKAGES = ""
SOC_PACKAGES_ccimx6 = "imx-gpu-viv-demos imx-gpu-viv-tools"

IMAGE_INSTALL = " \
    packagegroup-dey-core \
    ${ROOTFS_PKGMANAGE_BOOTSTRAP} \
    ${CORE_IMAGE_EXTRA_INSTALL} \
    ${SOC_PACKAGES} \
"

IMAGE_FEATURES += " \
    dey-network \
    dey-qt \
    package-management \
    ssh-server-dropbear \
    ${@bb.utils.contains('DISTRO_FEATURES', 'x11', 'x11-base x11-sato', '', d)} \
    ${@bb.utils.contains('MACHINE_FEATURES', 'accel-video', 'dey-gstreamer', '', d)} \
    ${@bb.utils.contains('MACHINE_FEATURES', 'alsa', 'dey-audio', '', d)} \
    ${@bb.utils.contains('MACHINE_FEATURES', 'bluetooth', 'dey-bluetooth', '', d)} \
    ${@bb.utils.contains('MACHINE_FEATURES', 'wifi', 'dey-wireless', '', d)} \
"

# Remove some features to reduce the rootfs size to fit in the internal storage.
IMAGE_FEATURES_remove_ccimx6ul = "package-management"

# SDK features (for toolchains generated from an image with populate_sdk)
SDKIMAGE_FEATURES ?= "dev-pkgs dbg-pkgs staticdev-pkgs"

IMAGE_LINGUAS = ""

inherit core-image
inherit dey-image
inherit distro_features_check

CONFLICT_DISTRO_FEATURES = "directfb wayland"

IMAGE_ROOTFS_SIZE = "8192"

# Do not install udev-cache
BAD_RECOMMENDATIONS += "udev-cache"

export IMAGE_BASENAME = "dey-image-qt-${GRAPHICAL_BACKEND}"
