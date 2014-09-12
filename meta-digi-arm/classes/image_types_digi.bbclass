inherit image_types_fsl

# Do not create static nodes in image files
USE_DEVFS = "1"

# Dynamically calculate max LEB count for UBIFS images
FLASH_MLC = "${@max_leb_count(d)}"
def max_leb_count(d):
    _mlc = []
    _flash_peb = d.getVar('FLASH_PEB', True)
    _flash_psz = d.getVar('FLASH_PSZ', True)
    for i in _flash_peb.split(','):
        _mlc.append(str(next_power_of_2(int(_flash_psz)/int(i)) - 1))
    return ','.join(_mlc)

# Return next power_of_2 bigger than passed argument
def next_power_of_2(n):
    i = 1
    while (n > i):
        i <<= 1
    return i

# Return TRUE if jffs2 is not in IMAGE_FSTYPES
JFFS2_NOT_IN_FSTYPES = "${@jffs2_not_in_fstypes(d)}"
def jffs2_not_in_fstypes(d):
    return str('jffs2' not in d.getVar('IMAGE_FSTYPES', True).split()).lower()

IMAGE_CMD_jffs2() {
	nimg="$(echo ${FLASH_PEB} | awk -F, '{print NF}')"
	for i in $(seq 1 ${nimg}); do
		peb_it="$(echo ${FLASH_PEB} | cut -d',' -f${i})"
		# Do not use '-p (padding)' option. It breaks 'ccardimx28js' flash images [JIRA:DEL-218]
		mkfs.jffs2 -n -e ${peb_it} -d ${IMAGE_ROOTFS} -o ${DEPLOY_DIR_IMAGE}/${IMAGE_NAME}.${peb_it}.rootfs.jffs2
		ln -sf ${IMAGE_NAME}.${peb_it}.rootfs.jffs2 ${DEPLOY_DIR_IMAGE}/${IMAGE_LINK_NAME}.${peb_it}.jffs2
	done
}

# The CWD for this set of commads is DEPLOY_DIR_IMAGE so the paths are relative to it.
COMPRESS_CMD_sum() {
	# 'nimg' is set in IMAGE_CMD_jffs2 (which is executed just before)
	for i in $(seq 1 ${nimg}); do
		peb_it="$(echo ${FLASH_PEB} | cut -d',' -f${i})"
		sumtool -e ${peb_it} -i ${IMAGE_NAME}.${peb_it}.rootfs.jffs2 -o ${IMAGE_NAME}.${peb_it}.rootfs.jffs2.sum
		ln -sf ${IMAGE_NAME}.${peb_it}.rootfs.jffs2.sum ${IMAGE_LINK_NAME}.${peb_it}.jffs2.sum

		# If 'jffs2' is not in IMAGE_FSTYPES remove the images and symlinks
		if ${JFFS2_NOT_IN_FSTYPES}; then
			rm -f ${IMAGE_NAME}.${peb_it}.rootfs.jffs2 ${IMAGE_LINK_NAME}.${peb_it}.jffs2
		fi
	done

	# Create dummy file so the final script can remove it and not fail
	if ${JFFS2_NOT_IN_FSTYPES}; then
		touch ${IMAGE_NAME}.rootfs.jffs2
	fi
}

IMAGE_CMD_ubifs() {
	nimg="$(echo ${FLASH_PEB} | awk -F, '{print NF}')"
	for i in $(seq 1 ${nimg}); do
		mlc_it="$(echo ${FLASH_MLC} | cut -d',' -f${i})"
		peb_it="$(echo ${FLASH_PEB} | cut -d',' -f${i})"
		leb_it="$(echo ${FLASH_LEB} | cut -d',' -f${i})"
		mio_it="$(echo ${FLASH_MIO} | cut -d',' -f${i})"
		mkfs.ubifs -r ${IMAGE_ROOTFS} -o ${DEPLOY_DIR_IMAGE}/${IMAGE_NAME}.${peb_it}.rootfs.ubifs -m ${mio_it} -e ${leb_it} -c ${mlc_it} ${MKUBIFS_ARGS}
		ln -sf ${IMAGE_NAME}.${peb_it}.rootfs.ubifs ${DEPLOY_DIR_IMAGE}/${IMAGE_LINK_NAME}.${peb_it}.ubifs
	done
}

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
