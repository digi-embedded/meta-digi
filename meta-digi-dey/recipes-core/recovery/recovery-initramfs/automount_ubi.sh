#!/bin/sh
#
# Copyright (c) 2023, Digi International Inc.
#

UPDATE_MOUNTPOINT="/mnt/update"
PARTITION_NAME="update"

# Check if there is a UBI volume called 'update'
# (for single MTD systems).
volname="$(ubinfo ${MDEV} | awk '$1=="Name:" {print $2}')"
if [ "${volname}" = "${PARTITION_NAME}" ]; then
	if mkdir -p ${UPDATE_MOUNTPOINT} && ! mountpoint -q ${UPDATE_MOUNTPOINT}; then
		# Mount the volume.
		if ! mount -t ubifs "${MDEV}" "${UPDATE_MOUNTPOINT}"; then
			echo "ERROR: Could not mount '${PARTITION_NAME}' partition"
			rmdir --ignore-fail-on-non-empty ${UPDATE_MOUNTPOINT}
		fi
	fi
fi
