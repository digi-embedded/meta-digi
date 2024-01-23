#
# Copyright (C) 2016-2024 Digi International.
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
    ssh-server-dropbear \
    ${@bb.utils.contains('DISTRO_FEATURES', 'gstreamer', 'dey-gstreamer', '', d)} \
    ${@bb.utils.contains('DISTRO_FEATURES', 'wayland', 'weston', \
       bb.utils.contains('DISTRO_FEATURES',     'x11', 'x11-base x11-sato', \
                                                       '', d), d)} \
    ${@bb.utils.contains('MACHINE_FEATURES', 'alsa', 'dey-audio', '', d)} \
    ${@bb.utils.contains('MACHINE_FEATURES', 'bluetooth', 'dey-bluetooth', '', d)} \
    ${@bb.utils.contains('MACHINE_FEATURES', 'wifi', 'dey-wireless', '', d)} \
"

# SDK features (for toolchains generated from an image with populate_sdk)
SDKIMAGE_FEATURES ?= "dev-pkgs dbg-pkgs staticdev-pkgs"

IMAGE_LINGUAS = ""

inherit core-image
inherit dey-image
inherit features_check

CONFLICT_DISTRO_FEATURES = "directfb"

IMAGE_ROOTFS_SIZE = "8192"

BAD_RECOMMENDATIONS += "eudev-hwdb"

DEFAULT_IMAGE_BASENAME ??= "dey-image-${GRAPHICAL_CORE}-${GRAPHICAL_BACKEND}"
export IMAGE_BASENAME = "${DEFAULT_IMAGE_BASENAME}"

CORE_IMAGE_EXTRA_INSTALL += " \
    ${@bb.utils.contains('DISTRO_FEATURES', 'x11 wayland', 'weston-xwayland', '', d)} \
    dey-examples-digiapix \
"
