#
# Copyright (C) 2014 Digi International.
#
DESCRIPTION = "DEY busybox only based image."

IMAGE_INSTALL= "\
	base-files \
	base-passwd \
	busybox \
	busybox-static-nodes \
	sysvinit \
	initscripts \
	${ROOTFS_PKGMANAGE_BOOTSTRAP} \
	${CORE_IMAGE_EXTRA_INSTALL} \
"

IMAGE_LINGUAS = " "

LICENSE = "MIT"

inherit core-image
inherit dey-image

IMAGE_ROOTFS_SIZE ?= "8192"

IMAGE_FSTYPES_remove = "ext4"
IMAGE_FSTYPES_append = " ext2"

BAD_RECOMMENDATIONS += "busybox-syslog"
