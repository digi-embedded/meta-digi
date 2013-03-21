#
# Copyright (C) 2012 Digi International.
#
DESCRIPTION = "DBL busybox based image."

VIRTUAL-RUNTIME_dev_manager ?= "busybox-mdev"

IMAGE_INSTALL = "packagegroup-dbl-core ${VIRTUAL-RUNTIME_dev_manager} ${ROOTFS_PKGMANAGE_BOOTSTRAP} ${CORE_IMAGE_EXTRA_INSTALL}"

IMAGE_LINGUAS = " "

LICENSE = "MIT"

inherit core-image
inherit del-image

INC_PR = "r0"
PR = "${INC_PR}"

# These features will move to the project's local.conf
# where they can be customized by platform.

# Only common features to remain here.
IMAGE_FEATURES += "ssh-server-dropbear"
IMAGE_FEATURES += "package-management"
IMAGE_FEATURES += "del-network"

# Machine dependant features
IMAGE_FEATURES += '${@base_contains("MACHINE_FEATURES", "alsa", "del-audio", "", d)}'
IMAGE_FEATURES += '${@base_contains("MACHINE_FEATURES", "accel-video", "del-gstreamer", "", d)}'
IMAGE_FEATURES += '${@base_contains("MACHINE_FEATURES", "wifi", "del-wireless", "", d)}'
IMAGE_FEATURES += '${@base_contains("MACHINE_FEATURES", "bluetooth", "del-bluetooth", "", d)}'

IMAGE_ROOTFS_SIZE = "8192"

ROOTFS_POSTPROCESS_COMMAND += "del_rootfs_tuning;"
