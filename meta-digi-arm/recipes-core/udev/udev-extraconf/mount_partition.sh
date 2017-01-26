#!/bin/sh
#===============================================================================
#
#  mount_partition.sh
#
#  Copyright (C) 2017 by Digi International Inc.
#  All rights reserved.
#
#  This program is free software; you can redistribute it and/or modify it
#  under the terms of the GNU General Public License version 2 as published by
#  the Free Software Foundation.
#
#  !Description: Attempt to mount the partition triggered by udev
#
#===============================================================================

MOUNT="/bin/mount"
PARTITION_NAME="${1}"
MOUNTPOINT="/mnt/${PARTITION_NAME}"

# Use 'silent' if util-linux's mount (busybox's does not support that option)
[ "$(readlink ${MOUNT})" = "/bin/mount.util-linux" ] && MOUNT="${MOUNT} -o silent"

if mkdir -p "${MOUNTPOINT}" && ! mountpoint -q "${MOUNTPOINT}"; then
	if [ "${SUBSYSTEM}" = "block" ]; then
		FSTYPE="$(blkid ${DEVNAME} | sed -e 's,.*TYPE="\([^"]\+\)".*,\1,g')"
		if ! mount ${FSTYPE:+-t ${FSTYPE}} "${DEVNAME}" "${MOUNTPOINT}"; then
			logger -t udev "ERROR: Could not mount '${PARTITION_NAME}' partition"
			rmdir --ignore-fail-on-non-empty ${MOUNTPOINT}
		fi
	elif [ "${SUBSYSTEM}" = "mtd" ]; then
		# Attach and get UBI device number
		dev_number="$(ubiattach -p ${DEVNAME} 2>/dev/null | sed -ne 's,.*device number \([0-9]\).*,\1,g;T;p' 2>/dev/null)"
		# Check if volume exists.
		if ubinfo "/dev/ubi${dev_number}" -N "${PARTITION_NAME}" >/dev/null 2>&1; then
			# Mount the volume.
			if ! mount -t ubifs "ubi${dev_number}:${PARTITION_NAME}" "${MOUNTPOINT}"; then
				logger -t udev "ERROR: Could not mount '${PARTITION_NAME}' partition"
				rmdir --ignore-fail-on-non-empty ${MOUNTPOINT}
			fi
		else
			logger -t udev "ERROR: Could not mount '${PARTITION_NAME}' partition, volume not found"
			rmdir --ignore-fail-on-non-empty ${MOUNTPOINT}
		fi
	fi
fi
