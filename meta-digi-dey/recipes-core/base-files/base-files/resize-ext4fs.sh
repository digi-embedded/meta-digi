#!/bin/sh
#===============================================================================
#
#  resize-ext4fs.sh
#
#  Copyright (C) 2014 by Digi International Inc.
#  All rights reserved.
#
#  This program is free software; you can redistribute it and/or modify it
#  under the terms of the GNU General Public License version 2 as published by
#  the Free Software Foundation.
#
#
#  !Description: Resize EXT4 filesystems to the size of the partition
#
#===============================================================================

get_emmc_block_device() {
	emmc_number="$(sed -ne 's,.*mmcblk\(.\)boot0.*,\1,g;T;p' /proc/partitions)"
	if [ -b "/dev/mmcblk${emmc_number}" ] &&
	   [ -b "/dev/mmcblk${emmc_number}boot0" ] &&
	   [ -b "/dev/mmcblk${emmc_number}boot1" ] &&
	   [ -b "/dev/mmcblk${emmc_number}rpmb" ]; then
		echo "/dev/mmcblk${emmc_number}"
	fi
}

RESIZE2FS="$(which resize2fs)"
EMMC_BLOCK_DEVICE="$(get_emmc_block_device)"
if [ -x "${RESIZE2FS}" -a -n "${EMMC_BLOCK_DEVICE}" ]; then
	PARTITIONS="$(blkid | sed -ne "{s,\(^${EMMC_BLOCK_DEVICE}[^:]\+\):.*TYPE=\"ext4\".*,\1,g;T;p}" | sort -u)"
	for i in ${PARTITIONS}; do
		if ! ${RESIZE2FS} ${i} 2>/dev/null; then
			echo "ERROR: resize2fs ${i}"
		fi
	done
fi
