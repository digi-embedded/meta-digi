# Copyright (C) 2016-2022, Digi International Inc.

SUMMARY = "Generate update package for SWUpdate"
SECTION = "base"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"

SRC_URI = " \
    file://sw-description \
    file://sw-description-uboot \
    file://swupdate_uboot_nand.sh \
    file://swupdate_uboot_mmc.sh \
"

inherit swupdate

IMAGE_DEPENDS = "${@get_baseimg_pn(d)}"

IMG_NAME = "${IMAGE_DEPENDS}"

SWUPDATE_IMAGES = " \
    ${IMG_NAME} \
    ${@oe.utils.ifelse(d.getVar('SWUPDATE_UBOOTIMG') == 'true', '${UBOOT_PREFIX}', '')} \
"

SOFTWARE_VERSION ?= "0.0.1"
DESCRIPTION = "${@oe.utils.ifelse(d.getVar('TRUSTFENCE_ENCRYPT_ROOTFS') == '1', 'Encrypted rootfs ${IMG_NAME} update', '${IMG_NAME} update')}"

UBOOT_EXT ?= ".${UBOOT_SUFFIX}"

UBOOTIMG_OFFSET ?= "${BOOTLOADER_SEEK_BOOT}"

SWUPDATE_STORAGE_MEDIA = "${@oe.utils.conditional('STORAGE_MEDIA', 'mmc', 'swupdate_uboot_mmc.sh', 'swupdate_uboot_nand.sh', d)}"

python () {
    img_fstypes = d.getVar('BOOTFS_EXT') + " " + d.getVar('ROOTFS_EXT')
    d.setVarFlag("SWUPDATE_IMAGES_FSTYPES", d.getVar('IMG_NAME'), img_fstypes)
    if (d.getVar('SWUPDATE_UBOOTIMG') == "true"):
        uboot_fstypes = d.getVar('UBOOT_EXT')
        d.setVarFlag("SWUPDATE_IMAGES_FSTYPES", d.getVar('UBOOT_PREFIX'), uboot_fstypes)
}

python do_swuimage:prepend() {
    import glob

    if (d.getVar('TRUSTFENCE_SIGN') == "1"):
        d.setVar('SWUPDATE_PRIVATE_KEY', glob.glob(d.getVar('SWUPDATE_PRIVATE_KEY_TEMPLATE'))[0])
}

def get_baseimg_pn(d):
    file_name = d.getVar('PN')
    return file_name[:file_name.find("-swu")]


# Dual boot partition names for eMMC or MTD
BOOT_DEV_NAME_A ?= "${@bb.utils.contains('STORAGE_MEDIA', 'mmc', '/dev/mmcblk0p1', 'linux_a', d)}"
BOOT_DEV_NAME_B ?= "${@bb.utils.contains('STORAGE_MEDIA', 'mmc', '/dev/mmcblk0p2', 'linux_b', d)}"
ROOTFS_DEV_NAME_A ?= "${@bb.utils.contains('STORAGE_MEDIA', 'mmc', '/dev/mmcblk0p3', 'rootfs_a', d)}"
ROOTFS_DEV_NAME_B ?= "${@bb.utils.contains('STORAGE_MEDIA', 'mmc', '/dev/mmcblk0p4', 'rootfs_b', d)}"

ROOTFS_TYPE = "${@bb.utils.contains('IMAGE_FEATURES', 'read-only-rootfs', 'squashfs', '', d)}"
do_unpack[postfuncs] += "fill_description"

fill_description() {
	if [ "${SWUPDATE_UBOOTIMG}" = "true" ]; then
		cp ${WORKDIR}/sw-description-uboot ${WORKDIR}/sw-description
		if [ "${TRUSTFENCE_DEK_PATH}" != "0" ] && [ "${TRUSTFENCE_DEK_PATH}" != "default" ]; then
			sed -i -e "s,##UBOOTIMG_ENC##,enc,g" "${WORKDIR}/sw-description"
		else
			sed -i -e "s,##UBOOTIMG_ENC##,normal,g" "${WORKDIR}/sw-description"
		fi
		sed -i -e "s,##UBOOTIMG_NAME##,${UBOOT_PREFIX}-${MACHINE}${UBOOT_EXT},g" "${WORKDIR}/sw-description"
		sed -i -e "s,##SWUPDATE_UBOOT_SCRIPT##,${SWUPDATE_STORAGE_MEDIA},g" "${WORKDIR}/sw-description"
		sed -i -e "s,##UBOOTIMG_OFFSET##,${UBOOTIMG_OFFSET},g" "${WORKDIR}/sw-description"
	fi
	sed -i -e "s,##BOOTIMG_NAME##,${IMG_NAME}-${MACHINE}${BOOTFS_EXT},g" "${WORKDIR}/sw-description"
	sed -i -e "s,##BOOT_DEV##,${BOOT_DEV_NAME},g" "${WORKDIR}/sw-description"
	sed -i -e "s,##ROOTIMG_NAME##,${IMG_NAME}-${MACHINE}${ROOTFS_EXT},g" "${WORKDIR}/sw-description"
	sed -i -e "s,##ROOTFS_DEV##,${ROOTFS_DEV_NAME_FINAL},g" "${WORKDIR}/sw-description"
	sed -i -e "s,##SW_VERSION##,${SOFTWARE_VERSION},g" "${WORKDIR}/sw-description"
	sed -i -e "s,##DESCRIPTION##,${DESCRIPTION},g" "${WORKDIR}/sw-description"

	# Dualboot description
	sed -i -e "s,##BOOT_DEV_A##,${BOOT_DEV_NAME_A},g" "${WORKDIR}/sw-description"
	sed -i -e "s,##BOOT_DEV_B##,${BOOT_DEV_NAME_B},g" "${WORKDIR}/sw-description"
	sed -i -e "s,##ROOTFS_DEV_A##,${ROOTFS_DEV_NAME_A},g" "${WORKDIR}/sw-description"
	sed -i -e "s,##ROOTFS_DEV_B##,${ROOTFS_DEV_NAME_B},g" "${WORKDIR}/sw-description"
	if [ -n "${@bb.utils.contains('IMAGE_FEATURES', 'read-only-rootfs', '1', '', d)}" ]; then
		sed -i '/compressed/d' "${WORKDIR}/sw-description"
	fi
	sed -i -e "s,##ROOTFS_TYPE##,${ROOTFS_TYPE},g" "${WORKDIR}/sw-description"

}
