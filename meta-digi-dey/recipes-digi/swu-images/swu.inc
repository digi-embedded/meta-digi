# Copyright (C) 2016-2023, Digi International Inc.

SUMMARY = "Generate update package for SWUpdate"
SECTION = "base"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"

SRC_URI = " \
    file://sw-description-images_template \
    file://sw-description-files_template \
    file://sw-description-rdiff_template \
    file://swupdate_uboot_nand.sh \
    file://swupdate_uboot_mmc.sh \
    file://image_template_mmc \
    file://image_template_nand \
    file://image_template_rdiff_mmc \
    file://image_template_rdiff_nand \
    file://file_template_uboot \
    file://script_template_uboot\
    file://update_images.sh \
    file://update_files.sh \
    file://update_rdiff.sh \
"

inherit swupdate dey-swupdate-common

# Avoid all 'SRC_URI' files to be included in the SWU image. Include only 'SWUPDATE_IMAGES' files and 'sw-description' (added by default).
INHIBIT_SWUPDATE_ADD_SRC_URI = "true"

# Files to include in the SWU update package.
SWUPDATE_IMAGES = " \
    ${@oe.utils.ifelse(d.getVar('SWUPDATE_IS_IMAGES_UPDATE') == 'true', '${IMG_NAME}', '')} \
    ${@oe.utils.ifelse(d.getVar('SWUPDATE_IS_FILES_UPDATE') == 'true', '${SWUPDATE_FILES_TARGZ_FILE_NAME}', '')} \
    ${@oe.utils.ifelse(d.getVar('SWUPDATE_IS_RDIFF_UPDATE') == 'true', '${IMG_NAME} ${SWUPDATE_RDIFF_ROOTFS_DELTA_FILE_NAME}', '')} \
    ${@oe.utils.ifelse(d.getVar('SWUPDATE_UBOOTIMG') == 'true', '${SWUPDATE_UBOOT_NAME} ${SWUPDATE_UBOOT_NAME_TFA} ${SWUPDATE_UBOOT_SCRIPT_NAME}', '')} \
    ${SWUPDATE_SCRIPT_NAME} \
"

# Associate images and file types.
python () {
    img_fstypes = d.getVar('BOOTFS_EXT')
    if d.getVar('SWUPDATE_IS_IMAGES_UPDATE') == "true":
        img_fstypes = img_fstypes + " " + d.getVar('ROOTFS_EXT')
    d.setVarFlag("SWUPDATE_IMAGES_FSTYPES", d.getVar('IMG_NAME'), img_fstypes)
    if (d.getVar('SWUPDATE_UBOOTIMG') == "true"):
        uboot_fstypes = d.getVar('SWUPDATE_UBOOT_EXT')
        d.setVarFlag("SWUPDATE_IMAGES_FSTYPES", d.getVar('SWUPDATE_UBOOT_PREFIX'), uboot_fstypes)
        if (d.getVar('DEY_SOC_VENDOR') == "STM"):
            uboot_tfa_fstypes = d.getVar('SWUPDATE_UBOOT_EXT_TFA')
            d.setVarFlag("SWUPDATE_IMAGES_FSTYPES", d.getVar('SWUPDATE_UBOOT_PREFIX_TFA'), uboot_tfa_fstypes)
}

# Execute extra tasks before creating SWU update package.
python do_swuimage:prepend() {
    import glob
    import os.path
    import shutil

    # Set signing key for trustfence enabled updates.
    if (d.getVar('TRUSTFENCE_ENABLED') == "1"):
        d.setVar('SWUPDATE_PRIVATE_KEY', glob.glob(d.getVar('SWUPDATE_PRIVATE_KEY_TEMPLATE'))[0])

    # Copy script file.
    updatescript = d.getVar('SWUPDATE_SCRIPT', True)
    imgdeploydir = d.getVar('DEPLOY_DIR_IMAGE', True)
    if "/" not in updatescript:
        workdir = d.getVar('WORKDIR', True)
        updatescript = os.path.join(workdir, updatescript)
    if os.path.isfile(updatescript):
        shutil.copyfile(updatescript, os.path.join(imgdeploydir, os.path.basename(updatescript)))

    # Copy U-Boot script file.
    if d.getVar('SWUPDATE_UBOOTIMG') == "true":
        uboot_updatescript = d.getVar('SWUPDATE_UBOOT_SCRIPT_NAME', True)
        if "/" not in uboot_updatescript:
            workdir = d.getVar('WORKDIR', True)
            uboot_updatescript = os.path.join(workdir, uboot_updatescript)
        if os.path.isfile(uboot_updatescript):
            shutil.copyfile(uboot_updatescript, os.path.join(imgdeploydir, os.path.basename(uboot_updatescript)))
}

# Upgrade available.
UPGRADE_AVAILABLE = ""
UPGRADE_AVAILABLE:ccimx6 = "1"

# Create and fill 'sw-description' file.
fill_description() {
	if [ "${SWUPDATE_IS_FILES_UPDATE}" = "true" ]; then
		cp ${WORKDIR}/sw-description-files_template ${WORKDIR}/sw-description
	elif [ "${SWUPDATE_IS_RDIFF_UPDATE}" = "true" ]; then
		cp ${WORKDIR}/sw-description-rdiff_template ${WORKDIR}/sw-description
	else
		cp ${WORKDIR}/sw-description-images_template ${WORKDIR}/sw-description
	fi

	# Add U-Boot sections if required.
	if [ "${SWUPDATE_UBOOTIMG}" = "true" ]; then
		# Add U-Boot files section.
		UBOOT_FILE_TEMPLATE="file_template_uboot"
		FILES_TEMPLATE="%s\n"
		if [ "${DEY_SOC_VENDOR}" = "STM" ]; then
			FILES_TEMPLATE="%s,\n%s\n"
		fi
		if [ "${SWUPDATE_IS_FILES_UPDATE}" != "true" ]; then
			FILES_TEMPLATE="            files: (\n${FILES_TEMPLATE}            );\n"
		else
			FILES_TEMPLATE="                ,\n${FILES_TEMPLATE}"
		fi
		if [ "${DEY_SOC_VENDOR}" = "STM" ]; then
			printf "${FILES_TEMPLATE}" \
                           "$(sed -e "s,##FILE_NAME##,${SWUPDATE_UBOOT_NAME},g" -e "s,##FILE_PATH##,/tmp/${SWUPDATE_UBOOT_NAME},g" "${UBOOT_FILE_TEMPLATE}")" \
                           "$(sed -e "s,##FILE_NAME##,${SWUPDATE_UBOOT_NAME_TFA},g" -e "s,##FILE_PATH##,/tmp/${SWUPDATE_UBOOT_NAME_TFA},g" "${UBOOT_FILE_TEMPLATE}")" \
                           > template_out.txt
		else
			printf "${FILES_TEMPLATE}" \
                           "$(sed -e "s,##FILE_NAME##,${SWUPDATE_UBOOT_NAME},g" -e "s,##FILE_PATH##,/tmp/${SWUPDATE_UBOOT_NAME},g" "${UBOOT_FILE_TEMPLATE}")" \
                           > template_out.txt
		fi
		sed -i -e "/##FILES_UBOOT##/r template_out.txt" -e "/##FILES_UBOOT##/d" "${WORKDIR}/sw-description"

		# Add U-Boot script section.
		UBOOT_SCRIPT_TEMPLATE="script_template_uboot"
		printf "                ,\n%s\n" \
                           "$(cat "${UBOOT_SCRIPT_TEMPLATE}")" \
                           > template_out.txt
		sed -i -e "/##SCRIPT_UBOOT##/r template_out.txt" -e "/##SCRIPT_UBOOT##/d" "${WORKDIR}/sw-description"
	else
		# Remove U-Boot entries if U-Boot is not being updated.
		sed -i -e "s,##FILES_UBOOT##,,g" "${WORKDIR}/sw-description"
		sed -i -e "s,##SCRIPT_UBOOT##,,g" "${WORKDIR}/sw-description"
	fi

	if [ "${SWUPDATE_IS_FILES_UPDATE}" = "true" ]; then
		rm -f template_out.txt
		return 0
	fi

	# Build image names.
	BOOT_IMAGE_NAME="${IMG_NAME}-${MACHINE}${BOOTFS_EXT}"
	ROOTFS_IMAGE_NAME="${IMG_NAME}-${MACHINE}${ROOTFS_EXT}"

	# Set correct image templates.
	BOOT_IMAGE_TEMPLATE="${SWUPDATE_IMAGES_IMAGE_TEMPLATE_FILE}"
	ROOTFS_IMAGE_TEMPLATE="${SWUPDATE_IMAGES_IMAGE_TEMPLATE_FILE}"
	if [ "${SWUPDATE_IS_RDIFF_UPDATE}" = "true" ]; then
		ROOTFS_IMAGE_TEMPLATE="${SWUPDATE_RDIFF_IMAGE_TEMPLATE_FILE}"
	fi

	# Add primary bank images section for dual boot systems.
	printf "%s,\n%s\n" \
            "$(sed -e "s,##IMG_NAME##,${BOOT_IMAGE_NAME},g" -e "s,##DEV##,${BOOT_DEV_NAME_A},g" -e "/compressed/d" "${BOOT_IMAGE_TEMPLATE}")" \
            "$(sed -e "s,##IMG_NAME##,${ROOTFS_IMAGE_NAME},g" -e "s,##DEV##,${ROOTFS_DEV_NAME_A},g" "${ROOTFS_IMAGE_TEMPLATE}")" \
            > template_out.txt
	sed -i -e "/##IMAGES_PRIMARY##/r template_out.txt" -e "/##IMAGES_PRIMARY##/d" "${WORKDIR}/sw-description"

	# Add secondary bank images section for dual boot systems.
	printf "%s,\n%s\n" \
            "$(sed -e "s,##IMG_NAME##,${BOOT_IMAGE_NAME},g" -e "s,##DEV##,${BOOT_DEV_NAME_B},g" -e "/compressed/d" "${BOOT_IMAGE_TEMPLATE}")" \
            "$(sed -e "s,##IMG_NAME##,${ROOTFS_IMAGE_NAME},g" -e "s,##DEV##,${ROOTFS_DEV_NAME_B},g" "${ROOTFS_IMAGE_TEMPLATE}")" \
            > template_out.txt
	sed -i -e "/##IMAGES_SECONDARY##/r template_out.txt" -e "/##IMAGES_SECONDARY##/d" "${WORKDIR}/sw-description"

	# Add images section for single boot systems.
	if [ "${SWUPDATE_IS_RDIFF_UPDATE}" != "true" ]; then
	    printf "%s,\n%s\n" \
                "$(sed -e "s,##IMG_NAME##,${BOOT_IMAGE_NAME},g" -e "s,##DEV##,${BOOT_DEV_NAME},g" -e "/compressed/d" "${BOOT_IMAGE_TEMPLATE}")" \
                "$(sed -e "s,##IMG_NAME##,${ROOTFS_IMAGE_NAME},g" -e "s,##DEV##,${ROOTFS_DEV_NAME_FINAL},g" "${ROOTFS_IMAGE_TEMPLATE}")" \
                > template_out.txt
	    sed -i -e "/##IMAGES_SINGLE##/r template_out.txt" -e "/##IMAGES_SINGLE##/d" "${WORKDIR}/sw-description"
	fi

	# Remove 'compressed' flag for read-only file systems as they use 'squashfs' images.
	if [ -n "${@bb.utils.contains('IMAGE_FEATURES', 'read-only-rootfs', '1', '', d)}" ]; then
		sed -i -e "/compressed/d" "${WORKDIR}/sw-description"
	fi

	# Clean dir.
	rm -f template_out.txt
}
fill_description[dirs] = "${DEPLOY_DIR_IMAGE} ${WORKDIR}"
do_unpack[postfuncs] += "fill_description"
