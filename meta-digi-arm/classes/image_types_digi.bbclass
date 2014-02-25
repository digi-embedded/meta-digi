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

IMAGE_CMD_jffs2() {
	nimg="$(echo ${FLASH_PEB} | awk -F, '{print NF}')"
	for i in $(seq 1 ${nimg}); do
		peb_it="$(echo ${FLASH_PEB} | cut -d',' -f${i})"
		# Do not use '-p (padding)' option. It breaks 'ccardimx28js' flash images [JIRA:DEL-218]
		mkfs.jffs2 -n -e ${peb_it} -d ${IMAGE_ROOTFS} -o ${DEPLOY_DIR_IMAGE}/${IMAGE_NAME}.${peb_it}.rootfs.jffs2
	done
}

IMAGE_CMD_sum.jffs2() {
	nimg="$(echo ${FLASH_PEB} | awk -F, '{print NF}')"
	for i in $(seq 1 ${nimg}); do
		peb_it="$(echo ${FLASH_PEB} | cut -d',' -f${i})"
		# Do not use '-p (padding)' option. It breaks 'ccardimx28js' flash images [JIRA:DEL-218]
		mkfs.jffs2 -n -e ${peb_it} -d ${IMAGE_ROOTFS} -o ${DEPLOY_DIR_IMAGE}/${IMAGE_NAME}.${peb_it}.rootfs.jffs2
		sumtool -e ${peb_it} -i ${DEPLOY_DIR_IMAGE}/${IMAGE_NAME}.${peb_it}.rootfs.jffs2 -o ${DEPLOY_DIR_IMAGE}/${IMAGE_NAME}.${peb_it}.rootfs.sum.jffs2
		rm -f ${DEPLOY_DIR_IMAGE}/${IMAGE_NAME}.${peb_it}.rootfs.jffs2
	done
}

IMAGE_CMD_ubifs() {
	nimg="$(echo ${FLASH_PEB} | awk -F, '{print NF}')"
	for i in $(seq 1 ${nimg}); do
		mlc_it="$(echo ${FLASH_MLC} | cut -d',' -f${i})"
		peb_it="$(echo ${FLASH_PEB} | cut -d',' -f${i})"
		leb_it="$(echo ${FLASH_LEB} | cut -d',' -f${i})"
		mio_it="$(echo ${FLASH_MIO} | cut -d',' -f${i})"
		mkfs.ubifs -r ${IMAGE_ROOTFS} -o ${DEPLOY_DIR_IMAGE}/${IMAGE_NAME}.${peb_it}.rootfs.ubifs -m ${mio_it} -e ${leb_it} -c ${mlc_it}
	done
}

#
# A copy of the original function in 'image_types.bbclass', just overriding the
# part of the symlinks generation so we can create more than one symlink (one per
# JFFS2 image)
#
runimagecmd_jffs2() {
	ROOTFS_SIZE=`du -ks ${IMAGE_ROOTFS} | awk '{base_size = $1 * ${IMAGE_OVERHEAD_FACTOR}; base_size = ((base_size > ${IMAGE_ROOTFS_SIZE} ? base_size : ${IMAGE_ROOTFS_SIZE}) + ${IMAGE_ROOTFS_EXTRA_SPACE}); if (base_size != int(base_size)) base_size = int(base_size + 1); base_size = base_size + ${IMAGE_ROOTFS_ALIGNMENT} - 1; base_size -= base_size % ${IMAGE_ROOTFS_ALIGNMENT}; print base_size }'`
	${cmd}

	# And create the symlinks
	#
	# The previous $\{cmd} expands to IMAGE_CMD_jffs2 so we have all the
	# needed variables available (nimg, pebX, etc)
	if [ -n "${IMAGE_LINK_NAME}" ]; then
		for type in ${subimages}; do
			for i in $(seq 1 ${nimg}); do
				peb_it="$(echo ${FLASH_PEB} | cut -d',' -f${i})"
				ln -sf ${IMAGE_NAME}.${peb_it}.rootfs.$type ${DEPLOY_DIR_IMAGE}/${IMAGE_LINK_NAME}.${peb_it}.$type
			done
		done
	fi
}

runimagecmd_sum.jffs2 = "${runimagecmd_jffs2}"
runimagecmd_ubifs = "${runimagecmd_jffs2}"

runimagecmd_boot.vfat() {
	#
	# Image generation code for image type 'boot.vfat'
	#
	BOOTIMG_FILES="$(readlink -e ${DEPLOY_DIR_IMAGE}/${KERNEL_IMAGETYPE}-${MACHINE})"
	BOOTIMG_FILES_SYMLINK="${DEPLOY_DIR_IMAGE}/${KERNEL_IMAGETYPE}-${MACHINE}"
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

	# Build VFAT boot image in copy the contents
	mkfs.vfat -n "Boot ${MACHINE}" -S 512 -C ${DEPLOY_DIR_IMAGE}/${IMAGE_NAME}.boot.vfat ${BOOTIMG_BLOCKS}
	# Copy files into the FAT image (renaming DTB's on the fly)
	for i in ${BOOTIMG_FILES_SYMLINK}; do
		mcopy -i ${DEPLOY_DIR_IMAGE}/${IMAGE_NAME}.boot.vfat ${i} ::/$(basename ${i} | sed -e '/dtb$/s,^${KERNEL_IMAGETYPE}-,,g')
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
