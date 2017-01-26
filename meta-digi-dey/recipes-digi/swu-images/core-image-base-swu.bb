# Copyright (C) 2016 Digi International.
SUMMARY = "Generate update package for SWUpdate"
SECTION = "base"
LICENSE = "GPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

SRC_URI = "file://sw-description"

inherit swupdate

IMAGE_DEPENDS = "core-image-base"

SWUPDATE_IMAGES = "core-image-base"

SOFTWARE_VERSION ?= "0.0.1"

BOOTFS_EXT ?= ".boot.vfat"
BOOTFS_EXT_ccimx6ul ?= ".boot.ubifs"
ROOTFS_EXT ?= ".ext4"
ROOTFS_EXT_ccimx6ul ?= ".ubifs"

python () {
    img_fstypes = d.getVar('BOOTFS_EXT', True) + " " + d.getVar('ROOTFS_EXT', True)
    d.setVarFlag("SWUPDATE_IMAGES_FSTYPES", "core-image-base", img_fstypes)
}

do_unpack[postfuncs] += "fill_description"

fill_description() {
	sed -i -e "s,##BOOTIMG_NAME##,core-image-base-${MACHINE}${BOOTFS_EXT},g" "${WORKDIR}/sw-description"
	sed -i -e "s,##ROOTIMG_NAME##,core-image-base-${MACHINE}${ROOTFS_EXT},g" "${WORKDIR}/sw-description"
	sed -i -e "s,##SW_VERSION##,${SOFTWARE_VERSION},g" "${WORKDIR}/sw-description"
}
