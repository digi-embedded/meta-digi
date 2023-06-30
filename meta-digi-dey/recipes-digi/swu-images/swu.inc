# Copyright (C) 2016-2023, Digi International Inc.

SUMMARY = "Generate update package for SWUpdate"
SECTION = "base"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"

SRC_URI = " \
    file://sw-description-images_template \
    file://sw-description-uboot \
    file://swupdate_uboot_nand.sh \
    file://swupdate_uboot_mmc.sh \
    file://image_template_mmc \
    file://image_template_nand \
    file://update_images.sh \
"

inherit swupdate

IMAGE_DEPENDS = "${@get_baseimg_pn(d)}"

IMG_NAME = "${IMAGE_DEPENDS}"

# Determine the correct UBoot update script file to use depending on storage type.
SWUPDATE_UBOOT_SCRIPT = "${@oe.utils.conditional('STORAGE_MEDIA', 'mmc', 'swupdate_uboot_mmc.sh', 'swupdate_uboot_nand.sh', d)}"

# Determine the storage type.
SWUPDATE_STORAGE_TYPE = "${@oe.utils.conditional('STORAGE_MEDIA', 'mmc', 'mmc', 'mtd', d)}"

# Avoid all 'SRC_URI' files to be included in the SWU image. Include only 'SWUPDATE_IMAGES' files and 'sw-description' (added by default).
INHIBIT_SWUPDATE_ADD_SRC_URI = "true"

SWUPDATE_IMAGES = " \
    ${IMG_NAME} \
    ${@oe.utils.ifelse(d.getVar('SWUPDATE_UBOOTIMG') == 'true', '${UBOOT_PREFIX}', '')} \
    ${@oe.utils.ifelse(d.getVar('SWUPDATE_UBOOTIMG') == 'true', '${SWUPDATE_UBOOT_SCRIPT}', '')} \
"

DESCRIPTION = "${@oe.utils.ifelse(d.getVar('TRUSTFENCE_ENCRYPT_ROOTFS') == '1', 'Encrypted rootfs ${IMG_NAME} update', '${IMG_NAME} update')}"

UBOOT_EXT ?= ".${UBOOT_SUFFIX}"

UBOOTIMG_OFFSET ?= "${BOOTLOADER_SEEK_BOOT}"

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

# Image template based on storage type.
IMAGE_TEMPLATE_FILE = "${@bb.utils.contains('STORAGE_MEDIA', 'mmc', '${WORKDIR}/image_template_mmc', '${WORKDIR}/image_template_nand', d)}"

# Update script.
SWUPDATE_SCRIPT ?= "${WORKDIR}/update_images.sh"
SWUPDATE_SCRIPT_NAME = "${@os.path.basename(d.getVar('SWUPDATE_SCRIPT'))}"
SWUPDATE_IMAGES += " ${SWUPDATE_SCRIPT_NAME}"

do_unpack[postfuncs] += "fill_description"

fill_description() {
	if [ "${SWUPDATE_UBOOTIMG}" = "true" ]; then
		cp ${WORKDIR}/sw-description-uboot ${WORKDIR}/sw-description
		cp ${WORKDIR}/${SWUPDATE_UBOOT_SCRIPT} ${DEPLOY_DIR_IMAGE}/${SWUPDATE_UBOOT_SCRIPT}
		if [ "${TRUSTFENCE_DEK_PATH}" != "0" ] && [ "${TRUSTFENCE_DEK_PATH}" != "default" ]; then
			sed -i -e "s,##UBOOTIMG_ENC##,enc,g" "${WORKDIR}/sw-description"
		else
			sed -i -e "s,##UBOOTIMG_ENC##,normal,g" "${WORKDIR}/sw-description"
		fi
		sed -i -e "s,##UBOOTIMG_NAME##,${UBOOT_PREFIX}-${MACHINE}${UBOOT_EXT},g" "${WORKDIR}/sw-description"
		sed -i -e "s,##SWUPDATE_UBOOT_SCRIPT##,${SWUPDATE_UBOOT_SCRIPT},g" "${WORKDIR}/sw-description"
		sed -i -e "s,##UBOOTIMG_OFFSET##,${UBOOTIMG_OFFSET},g" "${WORKDIR}/sw-description"
	else
		cp ${WORKDIR}/sw-description-images_template ${WORKDIR}/sw-description
	fi

	# Copy update script.
	if [ -f "${SWUPDATE_SCRIPT}" ]; then
		cp "${SWUPDATE_SCRIPT}" "${DEPLOY_DIR_IMAGE}"
	fi

	# Build image names.
	BOOT_IMAGE_NAME="${IMG_NAME}-${MACHINE}${BOOTFS_EXT}"
	ROOTFS_IMAGE_NAME="${IMG_NAME}-${MACHINE}${ROOTFS_EXT}"

	# Add primary bank images section for dual boot systems.
	printf "%s,\n%s\n" \
            "$(sed -e "s,##IMG_NAME##,${BOOT_IMAGE_NAME},g" -e "s,##DEV##,${BOOT_DEV_NAME_A},g" -e "/compressed/d" "${IMAGE_TEMPLATE_FILE}")" \
            "$(sed -e "s,##IMG_NAME##,${ROOTFS_IMAGE_NAME},g" -e "s,##DEV##,${ROOTFS_DEV_NAME_A},g" "${IMAGE_TEMPLATE_FILE}")" \
            > images_temp.txt
	sed -i -e "/##IMAGES_PRIMARY##/r images_temp.txt" -e "/##IMAGES_PRIMARY##/d" "${WORKDIR}/sw-description"

	# Add secondary bank images section for dual boot systems.
	printf "%s,\n%s\n" \
            "$(sed -e "s,##IMG_NAME##,${BOOT_IMAGE_NAME},g" -e "s,##DEV##,${BOOT_DEV_NAME_B},g" -e "/compressed/d" "${IMAGE_TEMPLATE_FILE}")" \
            "$(sed -e "s,##IMG_NAME##,${ROOTFS_IMAGE_NAME},g" -e "s,##DEV##,${ROOTFS_DEV_NAME_B},g" "${IMAGE_TEMPLATE_FILE}")" \
            > images_temp.txt
	sed -i -e "/##IMAGES_SECONDARY##/r images_temp.txt" -e "/##IMAGES_SECONDARY##/d" "${WORKDIR}/sw-description"

	# Add images section for single boot systems.
	printf "%s,\n%s\n" \
            "$(sed -e "s,##IMG_NAME##,${BOOT_IMAGE_NAME},g" -e "s,##DEV##,${BOOT_DEV_NAME},g" -e "/compressed/d" "${IMAGE_TEMPLATE_FILE}")" \
            "$(sed -e "s,##IMG_NAME##,${ROOTFS_IMAGE_NAME},g" -e "s,##DEV##,${ROOTFS_DEV_NAME_FINAL},g" "${IMAGE_TEMPLATE_FILE}")" \
            > images_temp.txt
	sed -i -e "/##IMAGES_SINGLE##/r images_temp.txt" -e "/##IMAGES_SINGLE##/d" "${WORKDIR}/sw-description"

	# Remove 'compressed' flag for read-only file systems as they use 'squashfs' images.
	if [ -n "${@bb.utils.contains('IMAGE_FEATURES', 'read-only-rootfs', '1', '', d)}" ]; then
		sed -i -e "/compressed/d" "${WORKDIR}/sw-description"
	fi

	# Clean dir.
	rm -f images_temp.txt
}
