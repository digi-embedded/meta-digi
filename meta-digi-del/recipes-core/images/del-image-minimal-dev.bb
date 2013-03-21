#
# Copyright (C) 2012 Digi International.
#
DESCRIPTION = "DEL busybox based image (non graphical) suitable for development work."

INC_PR = "r0"
PR = "${INC_PR}"

VIRTUAL-RUNTIME_dev_manager ?= "busybox-mdev"

IMAGE_INSTALL = "packagegroup-del-core ${VIRTUAL-RUNTIME_dev_manager} ${ROOTFS_PKGMANAGE_BOOTSTRAP} ${CORE_IMAGE_EXTRA_INSTALL}"

VIRTUAL-RUNTIME_accel-graphics = '${@base_contains("DISTRO_FEATURES", "x11", "", "amd-gpu-bin-mx51", d)}'
IMAGE_INSTALL_append_mx5 = " ${@base_contains('MACHINE_FEATURES', 'accel-graphics', '${VIRTUAL-RUNTIME_accel-graphics}', '', d)}"

IMAGE_LINGUAS = " "

LICENSE = "MIT"

inherit core-image
inherit del-image

# These features will move to the project's local.conf
# where they can be customized by platform.

# Only common features to remain here.
IMAGE_FEATURES += "ssh-server-dropbear"
IMAGE_FEATURES += "del-network"
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

IMAGE_ROOTFS_SIZE = "8192"

ROOTFS_POSTPROCESS_COMMAND += "del_rootfs_tuning;"
