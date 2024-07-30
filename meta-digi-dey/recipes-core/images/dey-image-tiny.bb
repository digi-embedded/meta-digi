#
# Copyright (C) 2014, Digi International Inc.
#
DESCRIPTION = "DEY busybox only based image."

IMAGE_INSTALL= "\
	base-files \
	base-passwd \
	busybox \
	sysvinit \
	initscripts \
	${CORE_IMAGE_EXTRA_INSTALL} \
"

IMAGE_LINGUAS = " "

LICENSE = "MIT"

inherit core-image
inherit dey-image

IMAGE_ROOTFS_SIZE ?= "8192"

IMAGE_FSTYPES:remove = "ext4"
IMAGE_FSTYPES:append = " ext2"
