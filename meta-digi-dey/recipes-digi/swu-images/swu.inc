# Copyright (C) 2016-2020 Digi International Inc.
SUMMARY = "Generate update package for SWUpdate"
SECTION = "base"
LICENSE = "GPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

SRC_URI = "file://sw-description"
SRC_URI_append_ccimx6 = " ${@oe.utils.ifelse(d.getVar('TRUSTFENCE_INITRAMFS_IMAGE', True), 'file://preinstall_swu.sh', '')}"
SRC_URI_append_ccimx6ul = " ${@oe.utils.ifelse(d.getVar('TRUSTFENCE_INITRAMFS_IMAGE', True), '', 'file://preinstall_swu.sh')}"
SRC_URI_append_ccimx8x = " ${@oe.utils.ifelse(d.getVar('TRUSTFENCE_INITRAMFS_IMAGE', True), 'file://preinstall_swu.sh', '')}"
SRC_URI_append_ccimx8m = " ${@oe.utils.ifelse(d.getVar('TRUSTFENCE_INITRAMFS_IMAGE', True), 'file://preinstall_swu.sh', '')}"

inherit swupdate

IMAGE_DEPENDS = "${@get_baseimg_pn(d)}"

IMG_NAME = "${IMAGE_DEPENDS}"

SWUPDATE_IMAGES = "${IMG_NAME}"

SOFTWARE_VERSION ?= "0.0.1"

BOOTFS_EXT ?= ".boot.vfat"
BOOTFS_EXT_ccimx6ul ?= ".boot.ubifs"
ROOTFS_EXT ?= ".ext4"
ROOTFS_EXT_ccimx6ul ?= ".ubifs"

BOOT_DEV_NAME ?= "/dev/mmcblk0p1"
BOOT_DEV_NAME_ccimx6ul ?= "linux"
ROOTFS_DEV_NAME ?= "/dev/mmcblk0p3"
ROOTFS_DEV_NAME_ccimx6ul ?= "rootfs"
ROOTFS_ENC_DEV = "/dev/mapper/cryptroot"
ROOTFS_ENC_DEV_ccimx6ul = "${ROOTFS_DEV_NAME}"
ROOTFS_DEV_NAME_FINAL = "${@oe.utils.ifelse(d.getVar('TRUSTFENCE_INITRAMFS_IMAGE', True), '${ROOTFS_ENC_DEV}', '${ROOTFS_DEV_NAME}')}"
PREINST_SCRIPT_TEMPLATE = "scripts: ( { filename = \\"preinstall_swu.sh\\"; type = \\"preinstall\\"; sha256 = \\"@preinstall_swu.sh\\"; \\x7D );"
PREINST_SCRIPT_DESC = ""
PREINST_SCRIPT_DESC_ccimx6 = "${@oe.utils.ifelse(d.getVar('TRUSTFENCE_INITRAMFS_IMAGE', True), '${PREINST_SCRIPT_TEMPLATE}', '')}"
PREINST_SCRIPT_DESC_ccimx6ul = "${@oe.utils.ifelse(d.getVar('TRUSTFENCE_INITRAMFS_IMAGE', True), '', '${PREINST_SCRIPT_TEMPLATE}')}"
PREINST_SCRIPT_DESC_ccimx8x = "${@oe.utils.ifelse(d.getVar('TRUSTFENCE_INITRAMFS_IMAGE', True), '${PREINST_SCRIPT_TEMPLATE}', '')}"
PREINST_SCRIPT_DESC_ccimx8m = "${@oe.utils.ifelse(d.getVar('TRUSTFENCE_INITRAMFS_IMAGE', True), '${PREINST_SCRIPT_TEMPLATE}', '')}"

python () {
    img_fstypes = d.getVar('BOOTFS_EXT', True) + " " + d.getVar('ROOTFS_EXT', True)
    d.setVarFlag("SWUPDATE_IMAGES_FSTYPES", d.getVar('IMG_NAME', True), img_fstypes)
}

python do_swuimage_prepend() {
    import glob

    if (d.getVar('TRUSTFENCE_SIGN', True) == "1"):
        d.setVar('SWUPDATE_PRIVATE_KEY', glob.glob(d.getVar('SWUPDATE_PRIVATE_KEY_TEMPLATE', True))[0])
}

def get_baseimg_pn(d):
    file_name = d.getVar('PN', True)
    return file_name[:file_name.find("-swu")]

do_unpack[postfuncs] += "fill_description"

fill_description() {
	sed -i -e "s,##BOOTIMG_NAME##,${IMG_NAME}-${MACHINE}${BOOTFS_EXT},g" "${WORKDIR}/sw-description"
	sed -i -e "s,##BOOT_DEV##,${BOOT_DEV_NAME},g" "${WORKDIR}/sw-description"
	sed -i -e "s,##ROOTIMG_NAME##,${IMG_NAME}-${MACHINE}${ROOTFS_EXT},g" "${WORKDIR}/sw-description"
	sed -i -e "s,##ROOTFS_DEV##,${ROOTFS_DEV_NAME_FINAL},g" "${WORKDIR}/sw-description"
	sed -i -e "s,##SW_VERSION##,${SOFTWARE_VERSION},g" "${WORKDIR}/sw-description"
	sed -i -e "s/##PREINSTALL_SCRIPT##/${PREINST_SCRIPT_DESC}/g" "${WORKDIR}/sw-description"
}
