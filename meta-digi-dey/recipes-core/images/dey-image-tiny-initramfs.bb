#
# Copyright (C) 2014 Digi International.
#
DESCRIPTION = "DEY busybox only based initramfs image."

include dey-image-tiny.bb

export IMAGE_BASENAME = "dey-image-tiny-initramfs"

IMAGE_FSTYPES = "${INITRAMFS_FSTYPES}"
IMAGE_FSTYPES_append = " rootfs.initramfs"

