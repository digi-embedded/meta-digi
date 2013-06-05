#
# Copyright (C) 2012 Digi International.
#
DESCRIPTION = "DEY busybox based image (non graphical)."

INC_PR = "r0"
PR = "${INC_PR}"

IMAGE_INSTALL = "packagegroup-dey-core ${ROOTFS_PKGMANAGE_BOOTSTRAP} ${CORE_IMAGE_EXTRA_INSTALL}"

VIRTUAL-RUNTIME_accel-graphics = '${@base_contains("DISTRO_FEATURES", "x11", "", "amd-gpu-bin-mx51", d)}'
IMAGE_INSTALL_append_mx5 = " ${@base_contains('MACHINE_FEATURES', 'accel-graphics', '${VIRTUAL-RUNTIME_accel-graphics}', '', d)}"

IMAGE_LINGUAS = " "

LICENSE = "MIT"

inherit core-image
inherit dey-image

# Only common features to remain here.
VIRTUAL_RUNTIME_ssh_server ?= "ssh-server-dropbear"
IMAGE_FEATURES += "${VIRTUAL_RUNTIME_ssh_server}"
IMAGE_FEATURES += "dey-network"
IMAGE_FEATURES += "package-management"

# Machine dependant features
IMAGE_FEATURES += '${@base_contains("MACHINE_FEATURES", "alsa", "dey-audio", "", d)}'
IMAGE_FEATURES += '${@base_contains("MACHINE_FEATURES", "accel-video", "dey-gstreamer", "", d)}'
IMAGE_FEATURES += '${@base_contains("MACHINE_FEATURES", "wifi", "dey-wireless", "", d)}'
IMAGE_FEATURES += '${@base_contains("MACHINE_FEATURES", "bluetooth", "dey-bluetooth", "", d)}'

IMAGE_ROOTFS_SIZE = "8192"

ROOTFS_POSTPROCESS_COMMAND += "dey_rootfs_tuning;"
