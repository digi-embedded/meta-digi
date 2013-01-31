#
# Copyright (C) 2012 Digi International.
#
DESCRIPTION = "DEL busybox based image (non graphical) suitable for development work."

IMAGE_INSTALL = "packagegroup-del-core ${ROOTFS_PKGMANAGE} ${CORE_IMAGE_EXTRA_INSTALL}"

IMAGE_LINGUAS = " "

LICENSE = "MIT"

inherit core-image
inherit del-image

# Only common features to remain here.
IMAGE_FEATURES += "ssh-server-dropbear"
IMAGE_FEATURES += "dev-pkgs"
IMAGE_FEATURES += "package-management"

# Machine dependant features
IMAGE_FEATURES += '${@base_contains("MACHINE_FEATURES", "alsa", "del-audio", "", d)}'
IMAGE_FEATURES += '${@base_contains("MACHINE_FEATURES", "accel-video", "del-gstreamer", "", d)}'
IMAGE_FEATURES += '${@base_contains("MACHINE_FEATURES", "wifi", "del-wireless", "", d)}'
IMAGE_FEATURES += '${@base_contains("MACHINE_FEATURES", "bluetooth", "del-bluetooth", "", d)}'

# Adding debug-tweaks will enable empty password login.
# Note that adding debug-tweaks to EXTRA_IMAGE_FEATURES will not
# allow for empty password logins.
IMAGE_FEATURES += "debug-tweaks"

ROOTFS_POSTPROCESS_COMMAND += "del_rootfs_tuning;"
