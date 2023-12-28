# Copyright (C) 2016-2023, Digi International Inc.

DESCRIPTION = "Recovery initramfs image"
LICENSE = "MIT"

PACKAGE_INSTALL = " \
    busybox \
    libubootenv-bin \
    ${@bb.utils.contains('STORAGE_MEDIA', 'mmc', 'e2fsprogs-mke2fs', '', d)} \
    ${@bb.utils.contains('STORAGE_MEDIA', 'mtd', 'mtd-utils-ubifs', '', d)} \
    psplash \
    recovery-initramfs \
    swupdate \
    trustfence-tool \
    wipe \
    ${@bb.utils.contains('DEY_SOC_VENDOR', 'NXP', bb.utils.contains('STORAGE_MEDIA', 'mtd', 'imx-kobs', '', d), '', d)} \
"

# Do not pollute the initrd image with rootfs features
IMAGE_FEATURES = ""
IMAGE_LINGUAS = ""

python() {
    d.setVar('IMAGE_FSTYPES', 'cpio.gz.u-boot.tf')
}

inherit core-image image_types

IMAGE_ROOTFS_SIZE = "8192"

# Remove some packages added via recommendations
BAD_RECOMMENDATIONS += " \
    openssl-bin \
    openssl-conf \
    openssl-ossl-module-legacy \
"

export IMAGE_BASENAME = "dey-image-recovery-initramfs"

initramfs_cleanup() {
	# Delete SWUPDATE postinst script
	rm -f ${IMAGE_ROOTFS}${sysconfdir}/rpm-postinsts/*swupdate*
}
ROOTFS_POSTPROCESS_COMMAND += "initramfs_cleanup; "
