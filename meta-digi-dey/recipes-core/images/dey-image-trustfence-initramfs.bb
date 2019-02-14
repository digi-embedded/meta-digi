DESCRIPTION = "Trustfence initramfs image"
LICENSE = "MIT"

PACKAGE_INSTALL = " \
    busybox \
    pv \
    trustfence-initramfs \
"

# Do not pollute the initrd image with rootfs features
IMAGE_FEATURES = ""
IMAGE_LINGUAS = ""

IMAGE_FSTYPES = "cpio.gz.u-boot.tf"
inherit core-image image_types

IMAGE_ROOTFS_SIZE = "8192"

# Remove some packages added via recommendations
BAD_RECOMMENDATIONS += " \
    openssl-conf \
"

export IMAGE_BASENAME = "dey-image-trustfence-initramfs"
