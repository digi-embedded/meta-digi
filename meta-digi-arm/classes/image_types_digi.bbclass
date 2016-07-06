inherit image_types

IMAGE_DEPENDS_boot.vfat = " \
    dosfstools-native:do_populate_sysroot \
    mtools-native:do_populate_sysroot \
    u-boot:do_deploy \
    virtual/kernel:do_deploy \
"

IMAGE_CMD_boot.vfat() {
	#
	# Image generation code for image type 'boot.vfat'
	#
	BOOTIMG_FILES="$(readlink -e ${DEPLOY_DIR_IMAGE}/${KERNEL_IMAGETYPE}-${MACHINE})"
	BOOTIMG_FILES_SYMLINK="${DEPLOY_DIR_IMAGE}/${KERNEL_IMAGETYPE}-${MACHINE}.bin"
	if [ -n "${KERNEL_DEVICETREE}" ]; then
		for DTB in ${KERNEL_DEVICETREE}; do
			if [ -e "${DEPLOY_DIR_IMAGE}/${KERNEL_IMAGETYPE}-${DTB}" ]; then
				BOOTIMG_FILES="${BOOTIMG_FILES} $(readlink -e ${DEPLOY_DIR_IMAGE}/${KERNEL_IMAGETYPE}-${DTB})"
				BOOTIMG_FILES_SYMLINK="${BOOTIMG_FILES_SYMLINK} ${DEPLOY_DIR_IMAGE}/${KERNEL_IMAGETYPE}-${DTB}"
			fi
		done
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
	mkfs.vfat -n "Boot ${MACHINE}" -S 512 -C ${DEPLOY_DIR_IMAGE}/${IMAGE_NAME}.boot.vfat ${BOOTIMG_BLOCKS}
	mcopy -i ${DEPLOY_DIR_IMAGE}/${IMAGE_NAME}.boot.vfat ${BOOTIMG_FILES_SYMLINK} ::/

	# Copy boot scripts into the VFAT image
	for item in ${BOOT_SCRIPTS}; do
		src=`echo $item | awk -F':' '{ print $1 }'`
		dst=`echo $item | awk -F':' '{ print $2 }'`
		mcopy -i ${DEPLOY_DIR_IMAGE}/${IMAGE_NAME}.boot.vfat -s ${DEPLOY_DIR_IMAGE}/$src ::/$dst
	done

	# Truncate the image to speed up the downloading/writing to the EMMC
	if [ -n "${BOARD_BOOTIMAGE_PARTITION_SIZE}" ]; then
		# U-Boot writes 512 bytes sectors so truncate the image at a sector boundary
		truncate -s $(expr \( \( ${BOOTIMG_FILES_SIZE} + 511 \) / 512 \) \* 512) ${DEPLOY_DIR_IMAGE}/${IMAGE_NAME}.boot.vfat
	fi

        # Create the symlink
	if [ -n "${IMAGE_LINK_NAME}" ] && [ -e ${DEPLOY_DIR_IMAGE}/${IMAGE_NAME}.boot.vfat ]; then
		ln -s ${IMAGE_NAME}.boot.vfat ${DEPLOY_DIR_IMAGE}/${IMAGE_LINK_NAME}.boot.vfat
	fi
}

IMAGE_CMD_boot.ubifs() {
	#
	# Image generation code for image type 'boot.ubifs'
	#
	BOOTIMG_FILES_SYMLINK="${DEPLOY_DIR_IMAGE}/${KERNEL_IMAGETYPE}-${MACHINE}.bin"
	if [ -n "${KERNEL_DEVICETREE}" ]; then
		for DTB in ${KERNEL_DEVICETREE}; do
			if [ -e "${DEPLOY_DIR_IMAGE}/${KERNEL_IMAGETYPE}-${DTB}" ]; then
				BOOTIMG_FILES_SYMLINK="${BOOTIMG_FILES_SYMLINK} ${DEPLOY_DIR_IMAGE}/${KERNEL_IMAGETYPE}-${DTB}"
			fi
		done
	fi

	# Create temporary folder
	TMP_BOOTDIR="$(mktemp -d ${DEPLOY_DIR_IMAGE}/boot.XXXXXX)"

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
	mkfs.ubifs -r ${TMP_BOOTDIR} -o ${DEPLOY_DIR_IMAGE}/${IMAGE_NAME}.boot.ubifs ${MKUBIFS_BOOT_ARGS}

	# Create the symlink
	if [ -n "${IMAGE_LINK_NAME}" ] && [ -e ${DEPLOY_DIR_IMAGE}/${IMAGE_NAME}.boot.ubifs ]; then
		ln -s ${IMAGE_NAME}.boot.ubifs ${DEPLOY_DIR_IMAGE}/${IMAGE_LINK_NAME}.boot.ubifs
	fi

	# Remove the temporary folder
	rm -rf ${TMP_BOOTDIR}
}

IMAGE_CMD_rootfs.initramfs() {
	#
	# Image generation code for image type 'rootfs.initramfs'
	#
	mkimage -A ${TARGET_ARCH} -O linux -T ramdisk -C none -n ${IMAGE_NAME} -d ${DEPLOY_DIR_IMAGE}/${IMAGE_NAME}.rootfs.cpio.gz ${DEPLOY_DIR_IMAGE}/${IMAGE_NAME}.rootfs.initramfs
	# Create the symlink
	if [ -n "${IMAGE_LINK_NAME}" ] && [ -e ${DEPLOY_DIR_IMAGE}/${IMAGE_NAME}.rootfs.initramfs ]; then
		ln -s ${IMAGE_NAME}.rootfs.initramfs ${DEPLOY_DIR_IMAGE}/${IMAGE_LINK_NAME}.initramfs
	fi
}
IMAGE_TYPEDEP_rootfs.initramfs = "cpio.gz"

# Set alignment to 4MB [in KiB]
IMAGE_ROOTFS_ALIGNMENT = "4096"

# Boot partition size in KiB, (default 64MiB)
BOARD_BOOTIMAGE_PARTITION_SIZE ??= "65536"

# SD card image name
SDIMG = "${DEPLOY_DIR_IMAGE}/${IMAGE_NAME}.rootfs.sdcard"

SDIMG_BOOTFS_TYPE ?= "boot.vfat"
SDIMG_BOOTFS = "${DEPLOY_DIR_IMAGE}/${IMAGE_NAME}.${SDIMG_BOOTFS_TYPE}"
SDIMG_ROOTFS_TYPE ?= "ext4"
SDIMG_ROOTFS = "${DEPLOY_DIR_IMAGE}/${IMAGE_NAME}.rootfs.${SDIMG_ROOTFS_TYPE}"

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

	# Burn bootloader, boot and rootfs partitions
	dd if=${DEPLOY_DIR_IMAGE}/${UBOOT_SYMLINK} of=${SDIMG} conv=notrunc,fsync seek=2 bs=512
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
