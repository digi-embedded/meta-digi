inherit image_types

################################################################################
#                                 BOOT IMAGES                                  #
################################################################################
# Boot partition size in KiB, (default 64MiB)
BOARD_BOOTIMAGE_PARTITION_SIZE ?= "65536"

def TRUSTFENCE_BOOTIMAGE_DEPENDS(d):
    tf_initramfs = d.getVar('TRUSTFENCE_INITRAMFS_IMAGE') or ""
    return "%s:do_image_complete" % tf_initramfs if tf_initramfs else ""

do_image_boot_vfat[depends] += " \
    coreutils-native:do_populate_sysroot \
    dosfstools-native:do_populate_sysroot \
    mtools-native:do_populate_sysroot \
    virtual/bootloader:do_deploy \
    virtual/kernel:do_deploy \
    ${@TRUSTFENCE_BOOTIMAGE_DEPENDS(d)} \
"

IMAGE_CMD:boot.vfat() {
	BOOTIMG_FILES="$(readlink -e ${DEPLOY_DIR_IMAGE}/${KERNEL_IMAGETYPE}-${MACHINE}.bin)"
	BOOTIMG_FILES_SYMLINK="${DEPLOY_DIR_IMAGE}/${KERNEL_IMAGETYPE}-${MACHINE}.bin"
	# Exclude DTB and DTBO from VFAT image when creating a FIT image
	if [ "${KERNEL_IMAGETYPE}" != "fitImage" ]; then
		if [ -n "${KERNEL_DEVICETREE}" ]; then
			for DTB in ${KERNEL_DEVICETREE}; do
				# Remove potential sub-folders
				DTB="$(basename ${DTB})"
				if [ -e "${DEPLOY_DIR_IMAGE}/${DTB}" ]; then
					BOOTIMG_FILES="${BOOTIMG_FILES} $(readlink -e ${DEPLOY_DIR_IMAGE}/${DTB})"
					BOOTIMG_FILES_SYMLINK="${BOOTIMG_FILES_SYMLINK} ${DEPLOY_DIR_IMAGE}/${DTB}"
				fi
			done
		fi
	fi

	# Add Trustfence initramfs if enabled
	if [ -n "${TRUSTFENCE_INITRAMFS_IMAGE}" ]; then
		BOOTIMG_FILES="${BOOTIMG_FILES} $(readlink -e ${DEPLOY_DIR_IMAGE}/${TRUSTFENCE_INITRAMFS_IMAGE}-${MACHINE}.cpio.gz.u-boot.tf)"
		BOOTIMG_FILES_SYMLINK="${BOOTIMG_FILES_SYMLINK} ${DEPLOY_DIR_IMAGE}/${TRUSTFENCE_INITRAMFS_IMAGE}-${MACHINE}.cpio.gz.u-boot.tf"
	fi

	# Size of kernel and device tree + 10% extra space (in bytes)
	BOOTIMG_FILES_SIZE="$(expr $(du -bc ${BOOTIMG_FILES} | tail -n1 | cut -f1) \* \( 100 + 10 \) / 100)"

	# 1KB blocks for mkfs.vfat
	BOOTIMG_BLOCKS="$(expr ${BOOTIMG_FILES_SIZE} / 1024)"
	if [ -n "${BOARD_BOOTIMAGE_PARTITION_SIZE}" ]; then
		BOOTIMG_BLOCKS="${BOARD_BOOTIMAGE_PARTITION_SIZE}"
	fi

	# POKY: Ensure total sectors is a multiple of sectors per track or mcopy will
	# complain. Blocks are 1024 bytes, sectors are 512 bytes, and we generate
	# images with 32 sectors per track. This calculation is done in blocks, thus
	# the use of 16 instead of 32.
	BOOTIMG_BLOCKS="$(expr \( \( ${BOOTIMG_BLOCKS} + 15 \) / 16 \) \* 16)"

	# Build VFAT boot image and copy files into it
	mkfs.vfat -n "Boot DEY" -S 512 -C ${IMGDEPLOYDIR}/${IMAGE_NAME}.boot.vfat ${BOOTIMG_BLOCKS}
	mcopy -i ${IMGDEPLOYDIR}/${IMAGE_NAME}.boot.vfat ${BOOTIMG_FILES_SYMLINK} ::/

	# Exclude boot scripts from VFAT image when creating a FIT image
	if [ "${KERNEL_IMAGETYPE}" != "fitImage" ]; then
		# Copy boot scripts into the VFAT image
		for item in ${BOOT_SCRIPTS}; do
			src=`echo $item | awk -F':' '{ print $1 }'`
			dst=`echo $item | awk -F':' '{ print $2 }'`
			mcopy -i ${IMGDEPLOYDIR}/${IMAGE_NAME}.boot.vfat -s ${DEPLOY_DIR_IMAGE}/$src ::/$dst
		done
	fi

	# Truncate the image to speed up the downloading/writing to the EMMC
	if [ -n "${BOARD_BOOTIMAGE_PARTITION_SIZE}" ]; then
		# U-Boot writes 512 bytes sectors so truncate the image at a sector boundary
		truncate -s $(expr \( \( ${BOOTIMG_FILES_SIZE} + 511 \) / 512 \) \* 512) ${IMGDEPLOYDIR}/${IMAGE_NAME}.boot.vfat
	fi
}

# Remove the default ".rootfs." suffix for 'boot.vfat' images
do_image_boot_vfat[imgsuffix] = "."

do_image_boot_ubifs[depends] += " \
    mtd-utils-native:do_populate_sysroot \
    virtual/bootloader:do_deploy \
    virtual/kernel:do_deploy \
    ${@TRUSTFENCE_BOOTIMAGE_DEPENDS(d)} \
"

IMAGE_CMD:boot.ubifs() {
	BOOTIMG_FILES_SYMLINK="${DEPLOY_DIR_IMAGE}/${KERNEL_IMAGETYPE}-${MACHINE}.bin"
	# Exclude DTB and DTBO from final image, when creating a FIT file
	if [ "${KERNEL_IMAGETYPE}" != "fitImage" ]; then
		if [ -n "${KERNEL_DEVICETREE}" ]; then
			for DTB in ${KERNEL_DEVICETREE}; do
				# Remove potential sub-folders
				DTB="$(basename ${DTB})"
				if [ -e "${DEPLOY_DIR_IMAGE}/${DTB}" ]; then
					BOOTIMG_FILES_SYMLINK="${BOOTIMG_FILES_SYMLINK} ${DEPLOY_DIR_IMAGE}/${DTB}"
				fi
			done
		fi
	fi

	# Add Trustfence initramfs if enabled
	if [ -n "${TRUSTFENCE_INITRAMFS_IMAGE}" ]; then
		BOOTIMG_FILES_SYMLINK="${BOOTIMG_FILES_SYMLINK} ${DEPLOY_DIR_IMAGE}/${TRUSTFENCE_INITRAMFS_IMAGE}-${MACHINE}.cpio.gz.u-boot.tf"
	fi

	# Create temporary folder
	TMP_BOOTDIR="$(mktemp -d ${IMGDEPLOYDIR}/boot.XXXXXX)"

	# Hard-link BOOTIMG_FILES into the temporary folder with the symlink filename
	for item in ${BOOTIMG_FILES_SYMLINK}; do
		orig="$(readlink -e ${item})"
		ln ${orig} ${TMP_BOOTDIR}/$(basename ${item})
	done

	# Exclude boot scripts from final image, when creating a FIT file
	if [ "${KERNEL_IMAGETYPE}" != "fitImage" ]; then
		# Hard-link boot scripts into the temporary folder
		for item in ${BOOT_SCRIPTS}; do
			src="$(echo ${item} | awk -F':' '{ print $1 }')"
			dst="$(echo ${item} | awk -F':' '{ print $2 }')"
			ln ${DEPLOY_DIR_IMAGE}/${src} ${TMP_BOOTDIR}/${dst}
		done
	fi

	# Build UBIFS boot image out of temp folder
	mkfs.ubifs -r ${TMP_BOOTDIR} -o ${IMGDEPLOYDIR}/${IMAGE_NAME}.boot.ubifs ${MKUBIFS_BOOT_ARGS}

	# Remove the temporary folder
	rm -rf ${TMP_BOOTDIR}
}

# Remove the default ".rootfs." suffix for 'boot.ubifs' images
do_image_boot_ubifs[imgsuffix] = "."

################################################################################
#                               RECOVERY IMAGES                                #
################################################################################
do_image_recovery_vfat[depends] +=  " \
    ${RECOVERY_INITRAMFS_IMAGE}:do_image_complete \
"

IMAGE_CMD:recovery.vfat() {
	# Use 'boot.vfat' image as base
	cp --remove-destination ${IMGDEPLOYDIR}/${IMAGE_NAME}.boot.vfat ${IMGDEPLOYDIR}/${IMAGE_NAME}.recovery.vfat

	# Exclude initRAMFS from VFAT image when creating a FIT image
	if [ "${KERNEL_IMAGETYPE}" != "fitImage" ]; then
		# Copy the recovery initramfs into the VFAT image
		mcopy -i ${IMGDEPLOYDIR}/${IMAGE_NAME}.recovery.vfat -s ${DEPLOY_DIR_IMAGE}/${RECOVERY_INITRAMFS_IMAGE}-${MACHINE}.cpio.gz.u-boot.tf ::/uramdisk-recovery.img
	fi
}

# Remove the default ".rootfs." suffix for 'recovery.vfat' images
do_image_recovery_vfat[imgsuffix] = "."

IMAGE_TYPEDEP:recovery.vfat = "boot.vfat"

do_image_recovery_ubifs[depends] += " \
    mtd-utils-native:do_populate_sysroot \
    virtual/bootloader:do_deploy \
    virtual/kernel:do_deploy \
    ${RECOVERY_INITRAMFS_IMAGE}:do_image_complete \
"

IMAGE_CMD:recovery.ubifs() {
	if [ "${KERNEL_IMAGETYPE}" = "fitImage" ]; then
		RECOVERYIMG_FILES_SYMLINK="${DEPLOY_DIR_IMAGE}/${KERNEL_IMAGETYPE}-${RECOVERY_INITRAMFS_IMAGE}-${MACHINE}-${MACHINE}"
	else
		RECOVERYIMG_FILES_SYMLINK="${DEPLOY_DIR_IMAGE}/${KERNEL_IMAGETYPE}-${MACHINE}.bin"
	fi
	# Exclude DTB and DTBO from final image, when creating a FIT file
	if [ "${KERNEL_IMAGETYPE}" != "fitImage" ]; then
		if [ -n "${KERNEL_DEVICETREE}" ]; then
			for DTB in ${KERNEL_DEVICETREE}; do
				# Remove potential sub-folders
				DTB="$(basename ${DTB})"
				if [ -e "${DEPLOY_DIR_IMAGE}/${DTB}" ]; then
					RECOVERYIMG_FILES_SYMLINK="${RECOVERYIMG_FILES_SYMLINK} ${DEPLOY_DIR_IMAGE}/${DTB}"
				fi
			done
		fi
	fi

	# Create temporary folder
	TMP_RECOVERYDIR="$(mktemp -d ${IMGDEPLOYDIR}/recovery.XXXXXX)"

	# Hard-link RECOVERYIMG_FILES into the temporary folder with the symlink filename
	for item in ${RECOVERYIMG_FILES_SYMLINK}; do
		orig="$(readlink -e ${item})"
		ln ${orig} ${TMP_RECOVERYDIR}/$(basename ${item})
	done

	# Exclude bootscript from final image when creating a FIT file
	if [ "${KERNEL_IMAGETYPE}" = "fitImage" ]; then
		# rename FITimage to u-boot default kernel load name
		mv ${TMP_RECOVERYDIR}/${KERNEL_IMAGETYPE}-${RECOVERY_INITRAMFS_IMAGE}-${MACHINE}-${MACHINE} ${TMP_RECOVERYDIR}/${KERNEL_IMAGETYPE}-${MACHINE}.bin
	else
		# Hard-link boot scripts into the temporary folder
		for item in ${BOOT_SCRIPTS}; do
			src="$(echo ${item} | awk -F':' '{ print $1 }')"
			dst="$(echo ${item} | awk -F':' '{ print $2 }')"
			ln ${DEPLOY_DIR_IMAGE}/${src} ${TMP_RECOVERYDIR}/${dst}
		done
		# Copy the recovery initramfs into the temporary folder
		cp ${DEPLOY_DIR_IMAGE}/${RECOVERY_INITRAMFS_IMAGE}-${MACHINE}.cpio.gz.u-boot.tf ${TMP_RECOVERYDIR}/uramdisk-recovery.img
	fi

	# Build UBIFS recovery image out of temp folder
	mkfs.ubifs -r ${TMP_RECOVERYDIR} -o ${IMGDEPLOYDIR}/${IMAGE_NAME}.recovery.ubifs ${MKUBIFS_RECOVERY_ARGS}

	# Remove the temporary folder
	rm -rf ${TMP_RECOVERYDIR}
}

# Remove the default ".rootfs." suffix for 'recovery.ubifs' images
do_image_recovery_ubifs[imgsuffix] = "."

################################################################################
#                               TRUSTFENCE SIGN                                #
################################################################################
trustence_sign_cpio() {
	#
	# Image generation code for image type 'cpio.gz.u-boot.tf'
	# (signed/encrypted ramdisk)
	#
	if [ "${TRUSTFENCE_SIGN_ARTIFACTS}" = "1" ] && [ "${TRUSTFENCE_SIGN_FIT_NXP}" = "0" ]; then
		# Set environment variables for trustfence configuration
		export CONFIG_SIGN_KEYS_PATH="${TRUSTFENCE_KEYS_PATH}"
		[ -n "${TRUSTFENCE_KEY_INDEX}" ] && export CONFIG_KEY_INDEX="${TRUSTFENCE_KEY_INDEX}"
		[ -n "${TRUSTFENCE_SRK_REVOKE_MASK}" ] && export SRK_REVOKE_MASK="${TRUSTFENCE_SRK_REVOKE_MASK}"
		[ "${TRUSTFENCE_ENCRYPT}" = "1" ] && export CONFIG_DEK_PATH="${TRUSTFENCE_KEYS_PATH}/${TRUSTFENCE_DEK_ENCRYPT_KEYNAME}"
		# Sign/encrypt the ramdisk
		trustfence-sign-artifact.sh -p "${DIGI_SOM}" -i "${1}" "${1}.tf"
	else
		# Copy the image with no changes
		cp "${1}" "${1}.tf"
	fi
}
CONVERSIONTYPES += "tf"
CONVERSION_CMD:tf = "trustence_sign_cpio ${IMAGE_NAME}.${type}"
CONVERSION_DEPENDS_tf = "${@oe.utils.conditional('TRUSTFENCE_SIGN', '1', 'trustfence-sign-tools-native', '', d)}"
IMAGE_TYPES += "cpio.gz.u-boot.tf"

#
# Sign read-only rootfs
#
do_image_squashfs[postfuncs] += "${@oe.utils.vartrue('TRUSTFENCE_SIGN_ARTIFACTS', 'rootfs_sign', '', d)}"
rootfs_sign() {
	# Set environment variables for trustfence configuration
	export CONFIG_SIGN_KEYS_PATH="${TRUSTFENCE_KEYS_PATH}"
	[ -n "${CONFIG_KEY_INDEX}" ] && export CONFIG_KEY_INDEX="${TRUSTFENCE_KEY_INDEX}"

	ROOTFS_IMAGE="${IMGDEPLOYDIR}/${IMAGE_NAME}.squashfs"
	TMP_ROOTFS_IMAGE_SIGNED="$(mktemp ${ROOTFS_IMAGE}-signed.XXXXXX)"
	# Sign rootfs read-only image
	trustfence-sign-artifact.sh -p "${DIGI_SOM}" -r "${ROOTFS_IMAGE}" "${TMP_ROOTFS_IMAGE_SIGNED}"
	mv "${TMP_ROOTFS_IMAGE_SIGNED}" "${ROOTFS_IMAGE}"
}
rootfs_sign[dirs] = "${DEPLOY_DIR_IMAGE}"

do_image_squashfs[vardeps] += "TRUSTFENCE_KEYS_PATH TRUSTFENCE_KEY_INDEX"
