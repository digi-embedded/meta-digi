inherit image_types_fsl

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
