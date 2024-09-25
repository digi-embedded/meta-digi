# Copyright (C) 2023, Digi International Inc.

pkg_postinst_ontarget:${PN}-mke2fs() {
	get_emmc_block_device() {
		for emmc_number in $(seq 0 9); do
			if [ -b "/dev/mmcblk${emmc_number}" ] &&
			   [ -b "/dev/mmcblk${emmc_number}boot0" ] &&
			   [ -b "/dev/mmcblk${emmc_number}boot1" ] &&
			   [ -c "/dev/mmcblk${emmc_number}rpmb" ]; then
				echo "/dev/mmcblk${emmc_number}"
				break
			fi
		done
	}

	get_emmc_data_device() {
		local EMMC_BLOCK_DEVICE="$(get_emmc_block_device)"
		if [ -n "${EMMC_BLOCK_DEVICE}" ]; then
			local DATA_DEVICE="$(blkid ${EMMC_BLOCK_DEVICE}p* | sed -ne "{s,\(^${EMMC_BLOCK_DEVICE}[^:]\+\):.*PARTLABEL=\"data\".*,\1,g;T;p}" | sort -u)"
			[ -n "${DATA_DEVICE}" ] && echo "${DATA_DEVICE}"
		fi
	}

	# Format and mount 'data' partition in block system based devices only if it has no format.
	EMMC_DATA_DEVICE="$(get_emmc_data_device)"
	if [ -n "${EMMC_DATA_DEVICE}" ] && ! blkid ${EMMC_DATA_DEVICE} | grep -q "TYPE="; then
		# Format the partition.
		echo "WARNING: 'data' partition has no format or it is invalid. Formatting..."
		if ! mkfs.ext4 "${EMMC_DATA_DEVICE}"; then
			echo "ERROR: Could not format 'data' partition"
		else
			# Trigger 'add' event for the partition.
			echo -n add > "/sys/class/block/${EMMC_DATA_DEVICE##*/}/uevent"
		fi
	fi
}

REMOVE_POSTINST_RPN = "${PN}-mke2fs"
inherit ${@bb.utils.contains("IMAGE_FEATURES", "read-only-rootfs", "remove-pkg-postinst-ontarget", "", d)}
