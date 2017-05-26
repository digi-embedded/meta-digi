inherit image_types

################################################################################
#                                 BOOT IMAGES                                  #
################################################################################
def TRUSTFENCE_BOOTIMAGE_DEPENDS(d):
    tf_initramfs = d.getVar('TRUSTFENCE_INITRAMFS_IMAGE',True) or ""
    return "%s:do_image_complete" % tf_initramfs if tf_initramfs else ""

IMAGE_DEPENDS_boot.vfat = " \
    dosfstools-native:do_populate_sysroot \
    mtools-native:do_populate_sysroot \
    u-boot:do_deploy \
    virtual/kernel:do_deploy \
    ${@TRUSTFENCE_BOOTIMAGE_DEPENDS(d)} \
"

IMAGE_CMD_boot.vfat() {
	BOOTIMG_FILES="$(readlink -e ${DEPLOY_DIR_IMAGE}/${KERNEL_IMAGETYPE}-${MACHINE}.bin)"
	BOOTIMG_FILES_SYMLINK="${DEPLOY_DIR_IMAGE}/${KERNEL_IMAGETYPE}-${MACHINE}.bin"
	if [ -n "${KERNEL_DEVICETREE}" ]; then
		for DTB in ${KERNEL_DEVICETREE}; do
			if [ -e "${DEPLOY_DIR_IMAGE}/${KERNEL_IMAGETYPE}-${DTB}" ]; then
				BOOTIMG_FILES="${BOOTIMG_FILES} $(readlink -e ${DEPLOY_DIR_IMAGE}/${KERNEL_IMAGETYPE}-${DTB})"
				BOOTIMG_FILES_SYMLINK="${BOOTIMG_FILES_SYMLINK} ${DEPLOY_DIR_IMAGE}/${KERNEL_IMAGETYPE}-${DTB}"
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

IMAGE_DEPENDS_boot.ubifs = " \
    mtd-utils-native:do_populate_sysroot \
    u-boot:do_deploy \
    virtual/kernel:do_deploy \
    ${@TRUSTFENCE_BOOTIMAGE_DEPENDS(d)} \
"

IMAGE_CMD_boot.ubifs() {
	BOOTIMG_FILES_SYMLINK="${DEPLOY_DIR_IMAGE}/${KERNEL_IMAGETYPE}-${MACHINE}.bin"
	if [ -n "${KERNEL_DEVICETREE}" ]; then
		for DTB in ${KERNEL_DEVICETREE}; do
			if [ -e "${DEPLOY_DIR_IMAGE}/${KERNEL_IMAGETYPE}-${DTB}" ]; then
				BOOTIMG_FILES_SYMLINK="${BOOTIMG_FILES_SYMLINK} ${DEPLOY_DIR_IMAGE}/${KERNEL_IMAGETYPE}-${DTB}"
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

#
# Transfer the dependences from the basetype 'boot' to the actual image types
#
# This is needed because otherwise the IMAGE_DEPENDS_<actualtype> is not used and the build fails.
#
IMAGE_DEPENDS_boot = " \
    ${@bb.utils.contains('IMAGE_FSTYPES', 'boot.ubifs', '${IMAGE_DEPENDS_boot.ubifs}', '', d)} \
    ${@bb.utils.contains('IMAGE_FSTYPES', 'boot.vfat', '${IMAGE_DEPENDS_boot.vfat}', '', d)} \
"

################################################################################
#                               RECOVERY IMAGES                                #
################################################################################
IMAGE_DEPENDS_recovery.vfat = " \
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

IMAGE_DEPENDS_recovery.ubifs = " \
    mtd-utils-native:do_populate_sysroot \
    u-boot:do_deploy \
    virtual/kernel:do_deploy \
    ${RECOVERY_INITRAMFS_IMAGE}:do_image_complete \
"

IMAGE_CMD_recovery.ubifs() {
	RECOVERYIMG_FILES_SYMLINK="${DEPLOY_DIR_IMAGE}/${KERNEL_IMAGETYPE}-${MACHINE}.bin"
	if [ -n "${KERNEL_DEVICETREE}" ]; then
		for DTB in ${KERNEL_DEVICETREE}; do
			if [ -e "${DEPLOY_DIR_IMAGE}/${KERNEL_IMAGETYPE}-${DTB}" ]; then
				RECOVERYIMG_FILES_SYMLINK="${RECOVERYIMG_FILES_SYMLINK} ${DEPLOY_DIR_IMAGE}/${KERNEL_IMAGETYPE}-${DTB}"
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

#
# Transfer the dependences from the basetype 'recovery' to the actual image types
#
# This is needed because otherwise the IMAGE_DEPENDS_<actualtype> is not used and the build fails.
#
IMAGE_DEPENDS_recovery = " \
    ${@bb.utils.contains('IMAGE_FSTYPES', 'recovery.ubifs', '${IMAGE_DEPENDS_recovery.ubifs}', '', d)} \
    ${@bb.utils.contains('IMAGE_FSTYPES', 'recovery.vfat', '${IMAGE_DEPENDS_recovery.vfat}', '', d)} \
"

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

		# Sign/encrypt the ramdisk
		trustfence-sign-kernel.sh -p "${DIGI_FAMILY}" -i "${1}" "${1}.tf"
	else
		# Rename image
		mv "${1}" "${1}.tf"
	fi
}
CONVERSIONTYPES += "gz.u-boot.tf"
CONVERSION_CMD_gz.u-boot.tf = "${CONVERSION_CMD_gz.u-boot}; trustence_sign_cpio ${IMAGE_NAME}.rootfs.${type}.gz.u-boot"
IMAGE_TYPES += "cpio.gz.u-boot.tf"

################################################################################
#                                SDCARD IMAGES                                 #
################################################################################
# Set alignment to 4MB [in KiB]
IMAGE_ROOTFS_ALIGNMENT = "4096"

# Boot partition size in KiB, (default 64MiB)
BOARD_BOOTIMAGE_PARTITION_SIZE ??= "65536"

# SD card image name
SDIMG = "${IMGDEPLOYDIR}/${IMAGE_NAME}.rootfs.sdcard"

SDIMG_BOOTFS_TYPE ?= "boot.vfat"
SDIMG_BOOTFS = "${IMGDEPLOYDIR}/${IMAGE_NAME}.${SDIMG_BOOTFS_TYPE}"
SDIMG_ROOTFS_TYPE ?= "ext4"
SDIMG_ROOTFS = "${IMGDEPLOYDIR}/${IMAGE_NAME}.rootfs.${SDIMG_ROOTFS_TYPE}"

IMAGE_DEPENDS_sdcard = " \
    dosfstools-native:do_populate_sysroot \
    mtools-native:do_populate_sysroot \
    parted-native:do_populate_sysroot \
    u-boot:do_deploy \
    virtual/kernel:do_deploy \
"

#
# Create an image that can by written onto a SD card using dd.
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
	SDIMG_UBOOT="${DEPLOY_DIR_IMAGE}/${UBOOT_SYMLINK}"
	if [ "${TRUSTFENCE_SIGN}" = "1" ]; then
		SDIMG_UBOOT="$(readlink -e ${SDIMG_UBOOT} | sed -e 's,u-boot-,u-boot-signed-,g')"
	fi

	# Burn bootloader, boot and rootfs partitions
	dd if=${SDIMG_UBOOT} of=${SDIMG} conv=notrunc,fsync seek=2 bs=512
	dd if=${SDIMG_BOOTFS} of=${SDIMG} conv=notrunc,fsync seek=1 bs=$(expr ${IMAGE_ROOTFS_ALIGNMENT} \* 1024)
	dd if=${SDIMG_ROOTFS} of=${SDIMG} conv=notrunc,fsync seek=1 bs=$(expr ${IMAGE_ROOTFS_ALIGNMENT} \* 1024 + ${BOOT_SPACE_ALIGNED} \* 1024)
}

#
# Create an image that can by written onto a SD card using dd (for ccardimx28 family)
#
# The disk layout used is:
#
#   1. Not partitioned  : reserved for bootloader (u-boot at 1MiB offset)
#   2. BOOT PARTITION   : kernel and device tree blobs
#   3. ROOTFS PARTITION : rootfs
#
#         4MiB             BOOT_SPACE                 ROOTFS_SIZE
#  <----------------> <--------------------> <------------------------------>
# +---+--------------+----------------------+--------------------------------+
# |   | U-BOOT (RAW) | BOOT PARTITION (FAT) | ROOTFS PARTITION (EXT4)        |
# +---+--------------+----------------------+--------------------------------+
# ^   ^              ^                      ^                                ^
# |   |              |                      |                                |
# 0 1MiB           4MiB             4MiB + BOOT_SPACE                   SDIMG_SIZE
#
IMAGE_CMD_sdcard_ccardimx28() {
	# Align boot partition and calculate total sdcard image size
	BOOT_SPACE_ALIGNED="$(expr \( \( ${BOARD_BOOTIMAGE_PARTITION_SIZE} + ${IMAGE_ROOTFS_ALIGNMENT} - 1 \) / ${IMAGE_ROOTFS_ALIGNMENT} \) \* ${IMAGE_ROOTFS_ALIGNMENT})"
	SDIMG_SIZE="$(expr ${IMAGE_ROOTFS_ALIGNMENT} + ${BOOT_SPACE_ALIGNED} + $ROOTFS_SIZE)"

	# Initialize sdcard image file
	dd if=/dev/zero of=${SDIMG} bs=1024 count=0 seek=${SDIMG_SIZE}

	#
	# Bootstream header for u-boot at 1M offset
	#
	# The offset is coded in bytes 29-32 in little-endian. The
	# value to set is the offset in 512 bytes blocks + 1.
	#
	# For 1M offset we can calculate the bytes:
	#
	# printf '%08x' 2049 | grep -o .. | tac | tr -d '\n'
	#
	BS_HDR="\x33\x22\x11\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x08\x00\x00\x00\x00\x00\x00"

	# Use 'printf' command and not shell builtins because hexadecimal
	# format does not work well with 'dash' shell
	PRINTF="$(which printf)"

	# Create partition table, boot partition (with bootable flag) and rootfs partition (to the end of the disk)
	parted -s ${SDIMG} mklabel msdos
	parted -s ${SDIMG} unit KiB mkpart primary 1024 ${IMAGE_ROOTFS_ALIGNMENT}
	parted -s ${SDIMG} unit KiB mkpart primary fat32 ${IMAGE_ROOTFS_ALIGNMENT} $(expr ${IMAGE_ROOTFS_ALIGNMENT} \+ ${BOOT_SPACE_ALIGNED})
	parted -s ${SDIMG} set 2 boot on
	parted -s ${SDIMG} -- unit KiB mkpart primary ext2 $(expr ${IMAGE_ROOTFS_ALIGNMENT} \+ ${BOOT_SPACE_ALIGNED}) -1s
	parted -s ${SDIMG} unit KiB print

	# Change partition type to 0x53 for mxs processor family and write bootstream header
	echo -n S | dd of=${SDIMG} bs=1 count=1 seek=450 conv=notrunc
	${PRINTF} "${BS_HDR}" | dd of=${SDIMG} bs=512 seek=$(expr 1024 \* 2) conv=notrunc,sync

	# Burn bootloader, boot and rootfs partitions
	dd if=${DEPLOY_DIR_IMAGE}/${UBOOT_SYMLINK} of=${SDIMG} conv=notrunc,fsync seek=$(expr 1024 \* 2 \+ 1) bs=512
	dd if=${SDIMG_BOOTFS} of=${SDIMG} conv=notrunc,fsync seek=1 bs=$(expr ${IMAGE_ROOTFS_ALIGNMENT} \* 1024)
	dd if=${SDIMG_ROOTFS} of=${SDIMG} conv=notrunc,fsync seek=1 bs=$(expr ${IMAGE_ROOTFS_ALIGNMENT} \* 1024 + ${BOOT_SPACE_ALIGNED} \* 1024)
}

# The sdcard image requires the boot and rootfs images to be built before
IMAGE_TYPEDEP_sdcard = "${SDIMG_BOOTFS_TYPE} ${SDIMG_ROOTFS_TYPE}"
