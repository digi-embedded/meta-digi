#
# Copyright (C) 2012 Digi International.
#
DESCRIPTION = "DEY busybox based image (non graphical)."

IMAGE_INSTALL = "packagegroup-dey-core ${ROOTFS_PKGMANAGE_BOOTSTRAP} ${CORE_IMAGE_EXTRA_INSTALL}"

IMAGE_LINGUAS = " "

LICENSE = "MIT"

inherit core-image
inherit dey-image
inherit distro_features_check

CONFLICT_DISTRO_FEATURES = "directfb wayland"

# Add 'x11' to CONFLICT_DISTRO_FEATURES for 'dey-image-minimal' family of recipes but
# not for 'dey-image-graphical' (NOTICE: dey-image-graphical recipe includes this one)
CONFLICT_DISTRO_FEATURES += "${@base_ifelse(d.getVar('PN', True).startswith('dey-image-minimal'), "x11", "")}"

# Only common features to remain here.
VIRTUAL_RUNTIME_ssh_server ?= "ssh-server-dropbear"
IMAGE_FEATURES += "${VIRTUAL_RUNTIME_ssh_server}"
IMAGE_FEATURES += "dey-network"
IMAGE_FEATURES += "package-management"

# Machine dependent features
IMAGE_FEATURES += '${@base_contains("MACHINE_FEATURES", "alsa", "dey-audio", "", d)}'
IMAGE_FEATURES += '${@base_contains("MACHINE_FEATURES", "accel-video", "dey-gstreamer", "", d)}'
IMAGE_FEATURES += '${@base_contains("MACHINE_FEATURES", "wifi", "dey-wireless", "", d)}'
IMAGE_FEATURES += '${@base_contains("MACHINE_FEATURES", "bluetooth", "dey-bluetooth", "", d)}'

# SDK features (for toolchains generated from an image with populate_sdk)
SDKIMAGE_FEATURES ?= "dev-pkgs dbg-pkgs staticdev-pkgs"

IMAGE_ROOTFS_SIZE = "8192"

# Do not install udev-cache
BAD_RECOMMENDATIONS += "udev-cache"
