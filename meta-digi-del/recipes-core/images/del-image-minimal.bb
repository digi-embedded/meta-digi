#
# Copyright (C) 2012 Digi International.
#
DESCRIPTION = "DEL busybox based image (non graphical)."

IMAGE_INSTALL = "packagegroup-del-core ${ROOTFS_PKGMANAGE_BOOTSTRAP} ${CORE_IMAGE_EXTRA_INSTALL}"

IMAGE_LINGUAS = " "

LICENSE = "MIT"

inherit core-image
inherit del-image

# These features will move to the project's local.conf
# where they can be customized by platform.

# Only common features to remain here.
IMAGE_FEATURES += "ssh-server-dropbear"
IMAGE_FEATURES += "del-network"

# Machine dependant features
IMAGE_FEATURES += '${@base_contains("MACHINE_FEATURES", "alsa", "del-audio", "", d)}'
IMAGE_FEATURES += '${@base_contains("MACHINE_FEATURES", "accel-video", "del-gstreamer", "", d)}'
IMAGE_FEATURES += '${@base_contains("MACHINE_FEATURES", "wifi", "del-wireless", "", d)}'

IMAGE_ROOTFS_SIZE = "8192"

# remove not needed ipkg informations
ROOTFS_POSTPROCESS_COMMAND += "remove_packaging_data_files; del_rootfs_tuning;"
