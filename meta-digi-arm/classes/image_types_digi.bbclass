inherit image_types_fsl

###################################################
## Platform data to be used in different scripts ##
###################################################
#
# <platform> <max_leb_cnt> <peb (KiB)> <leb (bytes)> <min-io-size (bytes)> <sub-page-size (bytes)>
# mlc, peb, leb, mio and sub might be a list of values (separated by commas)
#
# Values verified from actual modules with:
# ubiattach -m <rootfs_part_number> /dev/ubi_ctrl
#
# Max LEB count values calculated assuming following partition sizes:
#
#    max_leb_cnt="$(($(power_of_2 $((psize / peb))) - 1))"
#    psize = 524288 KiB; peb=128 KiB -> max_leb_cnt = 4095
#    psize = 524288 KiB; peb=512 KiB -> max_leb_cnt = 1023
#    psize = 262144 KiB; peb=128 KiB -> max_leb_cnt = 2047
#
load_platform_data() {
	while read _pl _mlc _peb _leb _mio _sub; do
		eval "${_pl}_mlc=\"$(echo ${_mlc} | tr ',' ' ')\""
		eval "${_pl}_peb=\"$(echo ${_peb} | tr ',' ' ')\""
		eval "${_pl}_leb=\"$(echo ${_leb} | tr ',' ' ')\""
		eval "${_pl}_mio=\"$(echo ${_mio} | tr ',' ' ')\""
		eval "${_pl}_sub=\"$(echo ${_sub} | tr ',' ' ')\""
	done<<-_EOF_
		ccardimx28js    2047         128        126976           2048         -
		ccimx51js       4095,1023    128,512    129024,520192    2048,4096    512,1024
		ccimx53js       4095,1023    128,512    129024,520192    2048,4096    512,1024
_EOF_
	# Set generic variables for current MACHINE
	nimg="$(eval echo \${${MACHINE}_peb} | wc -w)"
	for i in $(seq 1 ${nimg}); do
		eval mlc${i}="$(eval echo \${${MACHINE}_mlc} | cut -d' ' -f${i})"
		eval peb${i}="$(eval echo \${${MACHINE}_peb} | cut -d' ' -f${i})"
		eval leb${i}="$(eval echo \${${MACHINE}_leb} | cut -d' ' -f${i})"
		eval mio${i}="$(eval echo \${${MACHINE}_mio} | cut -d' ' -f${i})"
		eval sub${i}="$(eval echo \${${MACHINE}_sub} | cut -d' ' -f${i})"
	done
}

IMAGE_CMD_jffs2() {
	# Source platform data
	load_platform_data

	for i in $(seq 1 ${nimg}); do
		eval peb_it="\${peb${i}}"
		# Do not use '-p (padding)' option. It breaks 'ccardimx28js' flash images [JIRA:DEL-218]
		mkfs.jffs2 -n -e ${peb_it} -d ${IMAGE_ROOTFS} -o ${DEPLOY_DIR_IMAGE}/${IMAGE_NAME}.${peb_it}.rootfs.jffs2
	done
}

IMAGE_CMD_sum.jffs2() {
	# Source platform data
	load_platform_data

	for i in $(seq 1 ${nimg}); do
		eval peb_it="\${peb${i}}"
		# Do not use '-p (padding)' option. It breaks 'ccardimx28js' flash images [JIRA:DEL-218]
		mkfs.jffs2 -n -e ${peb_it} -d ${IMAGE_ROOTFS} -o ${DEPLOY_DIR_IMAGE}/${IMAGE_NAME}.${peb_it}.rootfs.jffs2
		sumtool -e ${peb_it} -i ${DEPLOY_DIR_IMAGE}/${IMAGE_NAME}.${peb_it}.rootfs.jffs2 -o ${DEPLOY_DIR_IMAGE}/${IMAGE_NAME}.${peb_it}.rootfs.sum.jffs2
		rm -f ${DEPLOY_DIR_IMAGE}/${IMAGE_NAME}.${peb_it}.rootfs.jffs2
	done
}

IMAGE_CMD_ubifs() {
	# Source platform data
	load_platform_data

	for i in $(seq 1 ${nimg}); do
		eval mlc_it="\${mlc${i}}"
		eval peb_it="\${peb${i}}"
		eval leb_it="\${leb${i}}"
		eval mio_it="\${mio${i}}"
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
				eval peb_it="\${peb${i}}"
				ln -s ${IMAGE_NAME}.${peb_it}.rootfs.$type ${DEPLOY_DIR_IMAGE}/${IMAGE_LINK_NAME}.${peb_it}.$type
			done
		done
	fi
}

runimagecmd_sum.jffs2 = "${runimagecmd_jffs2}"
runimagecmd_ubifs = "${runimagecmd_jffs2}"
