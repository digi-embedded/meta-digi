#
# Copyright (C) 2014 Digi International.
#
DESCRIPTION = "DEY busybox only based initramfs image."

include dey-image-tiny.bb

export IMAGE_BASENAME = "dey-image-tiny-initramfs"

IMAGE_FSTYPES = "cpio.gz.u-boot.tf"
inherit image_types
