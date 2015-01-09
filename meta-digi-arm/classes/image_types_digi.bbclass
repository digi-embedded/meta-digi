inherit image_types_fsl

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
		BOOTIMG_BLOCKS="$(expr ${BOARD_BOOTIMAGE_PARTITION_SIZE} / 1024)"
	fi

	# POKY: Ensure total sectors is a multiple of sectors per track or mcopy will
	# complain. Blocks are 1024 bytes, sectors are 512 bytes, and we generate
	# images with 32 sectors per track. This calculation is done in blocks, thus
	# the use of 16 instead of 32.
	BOOTIMG_BLOCKS="$(expr \( \( ${BOOTIMG_BLOCKS} + 15 \) / 16 \) \* 16)"

	# Build VFAT boot image and copy files into it
	mkfs.vfat -n "Boot ${MACHINE}" -S 512 -C ${DEPLOY_DIR_IMAGE}/${IMAGE_NAME}.boot.vfat ${BOOTIMG_BLOCKS}
	mcopy -i ${DEPLOY_DIR_IMAGE}/${IMAGE_NAME}.boot.vfat ${BOOTIMG_FILES_SYMLINK} ::/

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

IMAGE_CMD_rootfs.initramfs() {
	#
	# Image generation code for image type 'rootfs.initramfs'
	#
	mkimage -A ${ARCH} -O linux -T ramdisk -C none -n ${IMAGE_NAME} -d ${DEPLOY_DIR_IMAGE}/${IMAGE_NAME}.rootfs.cpio.gz ${DEPLOY_DIR_IMAGE}/${IMAGE_NAME}.rootfs.initramfs
	# Create the symlink
	if [ -n "${IMAGE_LINK_NAME}" ] && [ -e ${DEPLOY_DIR_IMAGE}/${IMAGE_NAME}.rootfs.initramfs ]; then
		ln -s ${IMAGE_NAME}.rootfs.initramfs ${DEPLOY_DIR_IMAGE}/${IMAGE_LINK_NAME}.initramfs
	fi
}
IMAGE_TYPEDEP_rootfs.initramfs = "cpio.gz"

SDCARD_GENERATION_COMMAND_ccardimx28js = "generate_ccardimx28js_sdcard"
#
# Create an image that can by written onto a SD card using dd for use with ccardimx28js
#
# External variables needed:
#   ${SDCARD_ROOTFS}    - the rootfs image to incorporate
#
# The disk layout used is:
#
#    1M                     -> IMAGE_ROOTFS_ALIGNMENT         - u-boot bootstream
#    IMAGE_ROOTFS_ALIGNMENT -> BOOT_SPACE                     - kernel + devide tree blob (VFAT partition)
#    BOOT_SPACE             -> SDIMG_SIZE                     - rootfs (EXT4 partition)
#
#                                                        Default Free space = 1.3x
#                                                        Use IMAGE_OVERHEAD_FACTOR to add more space
#                                                        <--------->
#            4MiB                8MiB             SDIMG_ROOTFS                    4MiB
# <-----------------------> <-------------> <----------------------> <------------------------------>
#  ---------------------------------------- ------------------------ -------------------------------
# |      |                 | BOOT_SPACE    | ROOTFS_SIZE            | IMAGE_ROOTFS_ALIGNMENT        |
#  ---------------------------------------- ------------------------ -------------------------------
# ^      ^                 ^               ^                        ^                               ^
# |      |                 |               |                        |                               |
# 0     1M                4M        4MiB + BOOTSPACE   4MiB + BOOTSPACE + SDIMG_ROOTFS   4MiB + BOOTSPACE + SDIMG_ROOTFS + 4MiB
#
generate_ccardimx28js_sdcard() {
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

	parted -s ${SDCARD} mklabel msdos
	parted -s ${SDCARD} unit KiB mkpart primary 1024 ${IMAGE_ROOTFS_ALIGNMENT}
	parted -s ${SDCARD} unit KiB mkpart primary fat32 ${IMAGE_ROOTFS_ALIGNMENT} $(expr ${IMAGE_ROOTFS_ALIGNMENT} \+ ${BOOT_SPACE_ALIGNED})
	parted -s ${SDCARD} unit KiB mkpart primary $(expr ${IMAGE_ROOTFS_ALIGNMENT} \+ ${BOOT_SPACE_ALIGNED}) $(expr ${IMAGE_ROOTFS_ALIGNMENT} \+ ${BOOT_SPACE_ALIGNED} \+ $ROOTFS_SIZE)

	# Change partition type to 0x53 for mxs processor family
	echo -n S | dd of=${SDCARD} bs=1 count=1 seek=450 conv=notrunc

	${PRINTF} "${BS_HDR}" | dd of=${SDCARD} bs=512 seek=$(expr 1024 \* 2) conv=notrunc,sync
	dd if=${DEPLOY_DIR_IMAGE}/u-boot-${MACHINE}.${UBOOT_SUFFIX} of=${SDCARD} bs=512 seek=$(expr 1024 \* 2 \+ 1) conv=notrunc,sync

	BOOT_BLOCKS=$(LC_ALL=C parted -s ${SDCARD} unit b print \
			| awk '/ 2 / { print substr($4, 1, length($4 -1)) / 1024 }')

	mkfs.vfat -n "${BOOTDD_VOLUME_ID}" -S 512 -C ${WORKDIR}/boot.img $BOOT_BLOCKS
	mcopy -i ${WORKDIR}/boot.img -s ${DEPLOY_DIR_IMAGE}/${KERNEL_IMAGETYPE}-${MACHINE}.bin ::/${KERNEL_IMAGETYPE}

	# Copy boot scripts
	for item in ${BOOT_SCRIPTS}; do
		src=`echo $item | awk -F':' '{ print $1 }'`
		dst=`echo $item | awk -F':' '{ print $2 }'`
		mcopy -i ${WORKDIR}/boot.img -s ${DEPLOY_DIR_IMAGE}/$src ::/$dst
	done

	# Copy kernel image and dtb's
	if test -n "${KERNEL_DEVICETREE}"; then
		for DTS_FILE in ${KERNEL_DEVICETREE}; do
			DTS_BASE_NAME=`basename ${DTS_FILE} | awk -F "." '{print $1}'`
			if [ -e "${KERNEL_IMAGETYPE}-${DTS_BASE_NAME}.dtb" ]; then
				kernel_bin="`readlink ${KERNEL_IMAGETYPE}-${MACHINE}.bin`"
				kernel_bin_for_dtb="`readlink ${KERNEL_IMAGETYPE}-${DTS_BASE_NAME}.dtb | sed "s,$DTS_BASE_NAME,${MACHINE},g;s,\.dtb$,.bin,g"`"
				if [ $kernel_bin = $kernel_bin_for_dtb ]; then
					mcopy -i ${WORKDIR}/boot.img -s ${DEPLOY_DIR_IMAGE}/${KERNEL_IMAGETYPE}-${DTS_BASE_NAME}.dtb ::/${DTS_BASE_NAME}.dtb
				fi
			fi
		done
	fi

	# Burn partitions
	dd if=${WORKDIR}/boot.img of=${SDCARD} conv=notrunc seek=1 bs=$(expr ${IMAGE_ROOTFS_ALIGNMENT} \* 1024) && sync && sync
	dd if=${SDCARD_ROOTFS} of=${SDCARD} conv=notrunc seek=1 bs=$(expr ${BOOT_SPACE_ALIGNED} \* 1024 + ${IMAGE_ROOTFS_ALIGNMENT} \* 1024) && sync && sync
}
