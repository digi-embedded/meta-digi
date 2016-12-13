# Copyright (C) 2016 Digi International.

DESCRIPTION = "Recovery initramfs image"
LICENSE = "MIT"

PACKAGE_INSTALL = " \
    busybox \
    recovery-initramfs \
    swupdate \
    trustfence-tool \
    u-boot-fw-utils \
    wipe \
"

PACKAGE_INSTALL_append_ccimx6 = " e2fsprogs-mke2fs parted"
PACKAGE_INSTALL_append_ccimx6ul = " mtd-utils-ubifs"

# Do not pollute the initrd image with rootfs features
IMAGE_FEATURES = ""
IMAGE_LINGUAS = ""

IMAGE_FSTYPES = "cpio.gz.u-boot.tf"
inherit core-image image_types_uboot

IMAGE_ROOTFS_SIZE = "8192"

# Remove some packages added via recommendations
BAD_RECOMMENDATIONS += " \
    busybox-syslog \
    openssl-conf \
"

export IMAGE_BASENAME = "dey-image-recovery-initramfs"
