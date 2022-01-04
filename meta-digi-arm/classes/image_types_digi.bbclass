inherit image_types

################################################################################
#                                 BOOT IMAGES                                  #
################################################################################
BOOTLOADER_IMAGE_RECIPE ?= "u-boot"

def TRUSTFENCE_BOOTIMAGE_DEPENDS(d):
    tf_initramfs = d.getVar('TRUSTFENCE_INITRAMFS_IMAGE',True) or ""
    return "%s:do_image_complete" % tf_initramfs if tf_initramfs else ""

do_image_boot_vfat[depends] += " \
    coreutils-native:do_populate_sysroot \
    dosfstools-native:do_populate_sysroot \
    mtools-native:do_populate_sysroot \
    ${BOOTLOADER_IMAGE_RECIPE}:do_deploy \
    virtual/kernel:do_deploy \
    ${@TRUSTFENCE_BOOTIMAGE_DEPENDS(d)} \
"

IMAGE_CMD_boot.vfat() {
	BOOTIMG_FILES="$(readlink -e ${DEPLOY_DIR_IMAGE}/${KERNEL_IMAGETYPE}-${MACHINE}.bin)"
	BOOTIMG_FILES_SYMLINK="${DEPLOY_DIR_IMAGE}/${KERNEL_IMAGETYPE}-${MACHINE}.bin"
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
	mkfs.vfat -n "Boot ${MACHINE}" -S 512 -C ${IMGDEPLOYDIR}/${IMAGE_NAME}.boot.vfat ${BOOTIMG_BLOCKS}
	mcopy -i ${IMGDEPLOYDIR}/${IMAGE_NAME}.boot.vfat ${BOOTIMG_FILES_SYMLINK} ::/

	# Copy boot scripts into the VFAT image
	for item in ${BOOT_SCRIPTS}; do
		src=`echo $item | awk -F':' '{ print $1 }'`
		dst=`echo $item | awk -F':' '{ print $2 }'`
		mcopy -i ${IMGDEPLOYDIR}/${IMAGE_NAME}.boot.vfat -s ${DEPLOY_DIR_IMAGE}/$src ::/$dst
	done

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
    ${BOOTLOADER_IMAGE_RECIPE}:do_deploy \
    virtual/kernel:do_deploy \
    ${@TRUSTFENCE_BOOTIMAGE_DEPENDS(d)} \
"

IMAGE_CMD_boot.ubifs() {
	BOOTIMG_FILES_SYMLINK="${DEPLOY_DIR_IMAGE}/${KERNEL_IMAGETYPE}-${MACHINE}.bin"
	if [ -n "${KERNEL_DEVICETREE}" ]; then
		for DTB in ${KERNEL_DEVICETREE}; do
			# Remove potential sub-folders
			DTB="$(basename ${DTB})"
			if [ -e "${DEPLOY_DIR_IMAGE}/${DTB}" ]; then
				BOOTIMG_FILES_SYMLINK="${BOOTIMG_FILES_SYMLINK} ${DEPLOY_DIR_IMAGE}/${DTB}"
			fi
		done
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

	# Hard-link boot scripts into the temporary folder
	for item in ${BOOT_SCRIPTS}; do
		src="$(echo ${item} | awk -F':' '{ print $1 }')"
		dst="$(echo ${item} | awk -F':' '{ print $2 }')"
		ln ${DEPLOY_DIR_IMAGE}/${src} ${TMP_BOOTDIR}/${dst}
	done

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

IMAGE_CMD_recovery.vfat() {
	# Use 'boot.vfat' image as base
	cp --remove-destination ${IMGDEPLOYDIR}/${IMAGE_NAME}.boot.vfat ${IMGDEPLOYDIR}/${IMAGE_NAME}.recovery.vfat

	# Copy the recovery initramfs into the VFAT image
	mcopy -i ${IMGDEPLOYDIR}/${IMAGE_NAME}.recovery.vfat -s ${DEPLOY_DIR_IMAGE}/${RECOVERY_INITRAMFS_IMAGE}-${MACHINE}.cpio.gz.u-boot.tf ::/uramdisk-recovery.img
}

# Remove the default ".rootfs." suffix for 'recovery.vfat' images
do_image_recovery_vfat[imgsuffix] = "."

IMAGE_TYPEDEP_recovery.vfat = "boot.vfat"

do_image_recovery_ubifs[depends] += " \
    mtd-utils-native:do_populate_sysroot \
    ${BOOTLOADER_IMAGE_RECIPE}:do_deploy \
    virtual/kernel:do_deploy \
    ${RECOVERY_INITRAMFS_IMAGE}:do_image_complete \
"

IMAGE_CMD_recovery.ubifs() {
	RECOVERYIMG_FILES_SYMLINK="${DEPLOY_DIR_IMAGE}/${KERNEL_IMAGETYPE}-${MACHINE}.bin"
	if [ -n "${KERNEL_DEVICETREE}" ]; then
		for DTB in ${KERNEL_DEVICETREE}; do
			# Remove potential sub-folders
			DTB="$(basename ${DTB})"
			if [ -e "${DEPLOY_DIR_IMAGE}/${DTB}" ]; then
				RECOVERYIMG_FILES_SYMLINK="${RECOVERYIMG_FILES_SYMLINK} ${DEPLOY_DIR_IMAGE}/${DTB}"
			fi
		done
	fi

	# Create temporary folder
	TMP_RECOVERYDIR="$(mktemp -d ${IMGDEPLOYDIR}/recovery.XXXXXX)"

	# Hard-link RECOVERYIMG_FILES into the temporary folder with the symlink filename
	for item in ${RECOVERYIMG_FILES_SYMLINK}; do
		orig="$(readlink -e ${item})"
		ln ${orig} ${TMP_RECOVERYDIR}/$(basename ${item})
	done

	# Hard-link boot scripts into the temporary folder
	for item in ${BOOT_SCRIPTS}; do
		src="$(echo ${item} | awk -F':' '{ print $1 }')"
		dst="$(echo ${item} | awk -F':' '{ print $2 }')"
		ln ${DEPLOY_DIR_IMAGE}/${src} ${TMP_RECOVERYDIR}/${dst}
	done

	# Copy the recovery initramfs into the temporary folder
	cp ${DEPLOY_DIR_IMAGE}/${RECOVERY_INITRAMFS_IMAGE}-${MACHINE}.cpio.gz.u-boot.tf ${TMP_RECOVERYDIR}/uramdisk-recovery.img

	# Build UBIFS recovery image out of temp folder
	mkfs.ubifs -r ${TMP_RECOVERYDIR} -o ${IMGDEPLOYDIR}/${IMAGE_NAME}.recovery.ubifs ${MKUBIFS_BOOT_ARGS}

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
	if [ "${TRUSTFENCE_SIGN}" = "1" ]; then
		# Set environment variables for trustfence configuration
		export CONFIG_SIGN_KEYS_PATH="${TRUSTFENCE_SIGN_KEYS_PATH}"
		[ -n "${TRUSTFENCE_KEY_INDEX}" ] && export CONFIG_KEY_INDEX="${TRUSTFENCE_KEY_INDEX}"
		[ -n "${TRUSTFENCE_DEK_PATH}" ] && [ "${TRUSTFENCE_DEK_PATH}" != "0" ] && export CONFIG_DEK_PATH="${TRUSTFENCE_DEK_PATH}"
		[ -n "${TRUSTFENCE_SIGN_MODE}" ] && export CONFIG_SIGN_MODE="${TRUSTFENCE_SIGN_MODE}"

		# Sign/encrypt the ramdisk
		trustfence-sign-artifact.sh -p "${DIGI_FAMILY}" -i "${1}" "${1}.tf"
	else
		# Copy the image with no changes
		cp "${1}" "${1}.tf"
	fi
}
CONVERSIONTYPES += "tf"
CONVERSION_CMD_tf = "trustence_sign_cpio ${IMAGE_NAME}.rootfs.${type}"
CONVERSION_DEPENDS_tf = "${@oe.utils.conditional('TRUSTFENCE_SIGN', '1', 'trustfence-sign-tools-native', '', d)}"
IMAGE_TYPES += "cpio.gz.u-boot.tf"

#
# Sign read-only rootfs
#
do_image_squashfs[postfuncs] += "${@oe.utils.conditional('TRUSTFENCE_SIGN', '1', 'rootfs_sign', '', d)}"

rootfs_sign() {
        # Set environment variables for trustfence configuration
        export CONFIG_SIGN_KEYS_PATH="${TRUSTFENCE_SIGN_KEYS_PATH}"
        [ -n "${CONFIG_KEY_INDEX}" ] && export CONFIG_KEY_INDEX="${TRUSTFENCE_KEY_INDEX}"
        [ -n "${CONFIG_SIGN_MODE}" ] && export CONFIG_SIGN_MODE="${TRUSTFENCE_SIGN_MODE}"

        ROOTFS_IMAGE="${IMGDEPLOYDIR}/${IMAGE_NAME}.rootfs.squashfs"
        TMP_ROOTFS_IMAGE_SIGNED="$(mktemp ${ROOTFS_IMAGE}-signed.XXXXXX)"
        # Sign rootfs read-only image
        trustfence-sign-artifact.sh -p "${DIGI_FAMILY}" -r "${ROOTFS_IMAGE}" "${TMP_ROOTFS_IMAGE_SIGNED}"
        mv "${TMP_ROOTFS_IMAGE_SIGNED}" "${ROOTFS_IMAGE}"
}

rootfs_sign[dirs] = "${DEPLOY_DIR_IMAGE}"

do_image_squashfs[vardeps] += "TRUSTFENCE_SIGN_KEYS_PATH TRUSTFENCE_KEY_INDEX"

################################################################################
#                                SDCARD IMAGES                                 #
################################################################################
# Set alignment to 4MB [in KiB]
IMAGE_ROOTFS_ALIGNMENT = "4096"

# Boot partition size in KiB, (default 64MiB)
BOARD_BOOTIMAGE_PARTITION_SIZE ??= "65536"

# SD card image name
SDIMG = "${IMGDEPLOYDIR}/${IMAGE_NAME}.rootfs.sdcard"

BOOTLOADER_SEEK ?= "1"

SDIMG_BOOTLOADER ?= "${DEPLOY_DIR_IMAGE}/${UBOOT_SYMLINK}"
SDIMG_BOOTFS_TYPE ?= "boot.vfat"
SDIMG_BOOTFS = "${IMGDEPLOYDIR}/${IMAGE_NAME}.${SDIMG_BOOTFS_TYPE}"
SDIMG_ROOTFS_TYPE ?= "ext4"
SDIMG_ROOTFS = "${IMGDEPLOYDIR}/${IMAGE_NAME}.rootfs.${SDIMG_ROOTFS_TYPE}"

do_image_sdcard[depends] = " \
    dosfstools-native:do_populate_sysroot \
    mtools-native:do_populate_sysroot \
    parted-native:do_populate_sysroot \
    ${BOOTLOADER_IMAGE_RECIPE}:do_deploy \
    virtual/kernel:do_deploy \
"

#
# Create an image that can be written onto an SD card using dd.
#
# The disk layout used is:
#
#   1. Not partitioned  : reserved for bootloader (u-boot)
#   2. BOOT PARTITION   : kernel and device tree blobs
#   3. ROOTFS PARTITION : rootfs
#
#       4MiB            BOOT_SPACE                 ROOTFS_SIZE
#  <------------> <--------------------> <------------------------------>
# +--------------+----------------------+--------------------------------+
# | U-BOOT (RAW) | BOOT PARTITION (FAT) | ROOTFS PARTITION (EXT4)        |
# +--------------+----------------------+--------------------------------+
# ^              ^                      ^                                ^
# |              |                      |                                |
# 0            4MiB             4MiB + BOOT_SPACE                   SDIMG_SIZE
#
IMAGE_CMD_sdcard() {
	# Align boot partition and calculate total sdcard image size
	BOOT_SPACE_ALIGNED="$(expr \( \( ${BOARD_BOOTIMAGE_PARTITION_SIZE} + ${IMAGE_ROOTFS_ALIGNMENT} - 1 \) / ${IMAGE_ROOTFS_ALIGNMENT} \) \* ${IMAGE_ROOTFS_ALIGNMENT})"
	SDIMG_SIZE="$(expr ${IMAGE_ROOTFS_ALIGNMENT} + ${BOOT_SPACE_ALIGNED} + $ROOTFS_SIZE)"

	# Initialize sdcard image file
	dd if=/dev/zero of=${SDIMG} bs=1024 count=0 seek=${SDIMG_SIZE}

	# Create partition table, boot partition (with bootable flag) and rootfs partition (to the end of the disk)
	parted -s ${SDIMG} mklabel msdos
	parted -s ${SDIMG} unit KiB mkpart primary fat32 ${IMAGE_ROOTFS_ALIGNMENT} $(expr ${IMAGE_ROOTFS_ALIGNMENT} \+ ${BOOT_SPACE_ALIGNED})
	parted -s ${SDIMG} set 1 boot on
	parted -s ${SDIMG} -- unit KiB mkpart primary ext2 $(expr ${IMAGE_ROOTFS_ALIGNMENT} \+ ${BOOT_SPACE_ALIGNED}) -1s
	parted -s ${SDIMG} unit KiB print

	# Set u-boot image to flash depending on whether TRUSTFENCE_SIGN is enabled
	if [ "${TRUSTFENCE_SIGN}" = "1" ]; then
		if [ "${BOOTLOADER_IMAGE_RECIPE}" = "u-boot" ]; then
			SDIMG_BOOT="$(readlink -e ${SDIMG_BOOTLOADER} | sed -e 's,u-boot-,u-boot-dtb-signed-,g')"
		else
			SDIMG_BOOT="$(readlink -e ${SDIMG_BOOTLOADER} | sed -e 's,imx-boot-,imx-boot-signed-,g')"
		fi
	else
		SDIMG_BOOT="$(readlink -e ${SDIMG_BOOTLOADER})"
	fi

	# Decompress rootfs image
	gzip -d -k ${SDIMG_ROOTFS}.gz

	# Burn bootloader, boot and rootfs partitions
	dd if=${SDIMG_BOOT} of=${SDIMG} conv=notrunc,fsync seek=${BOOTLOADER_SEEK} bs=1K
	dd if=${SDIMG_BOOTFS} of=${SDIMG} conv=notrunc,fsync seek=1 bs=$(expr ${IMAGE_ROOTFS_ALIGNMENT} \* 1024)
	dd if=${SDIMG_ROOTFS} of=${SDIMG} conv=notrunc,fsync seek=1 bs=$(expr ${IMAGE_ROOTFS_ALIGNMENT} \* 1024 + ${BOOT_SPACE_ALIGNED} \* 1024)

	# Delete the decompressed rootfs image
	rm -f ${SDIMG_ROOTFS}
}

# The sdcard image requires the boot and rootfs images to be built before
IMAGE_TYPEDEP_sdcard = "${SDIMG_BOOTFS_TYPE} ${SDIMG_ROOTFS_TYPE}.gz"

