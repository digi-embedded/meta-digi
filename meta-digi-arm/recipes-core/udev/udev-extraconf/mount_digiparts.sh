#!/bin/sh
#===============================================================================
#
#  mount_bootparts.sh
#
#  Copyright (C) 2014-2022 by Digi International Inc.
#  All rights reserved.
#
#  This program is free software; you can redistribute it and/or modify it
#  under the terms of the GNU General Public License version 2 as published by
#  the Free Software Foundation.
#
#  !Description: Attempt to mount boot partitions read-only (called from udev)
#
#===============================================================================

BASE_INIT="$(readlink -f "@base_sbindir@/init")"
INIT_SYSTEMD="@systemd_unitdir@/systemd"

if [ "${SUBSYSTEM}" = "block" ]; then
	PARTNAME="${ID_PART_ENTRY_NAME}"
elif [ "${SUBSYSTEM}" = "mtd" ]; then
	MTDN="$(echo ${DEVNAME} | cut -f 3 -d /)"
	PARTNAME="$(grep ${MTDN} /proc/mtd | sed -ne 's,.*"\(.*\)",\1,g;T;p')"
elif [ "${SUBSYSTEM}" = "ubi" ]; then
	PARTNAME="$(cat /sys/${DEVPATH}/name)"
fi

MOUNT_PARAMS="-o silent"
# Mount 'linux' partition as read-only
if [ "${PARTNAME}" = "linux" ]; then
	MOUNT_PARAMS="${MOUNT_PARAMS} -o ro"
fi

if [ "x$BASE_INIT" = "x$INIT_SYSTEMD" ];then
	# systemd as init uses systemd-mount to mount block devices
	MOUNT="/usr/bin/systemd-mount"
	MOUNT_PARAMS="${MOUNT_PARAMS} --no-block"

	if [ -x "$MOUNT" ];
	then
		logger "Using systemd-mount to finish mount"
	else
		logger "Linux init is using systemd, so please install systemd-mount to finish mount"
		exit 1
	fi
else
	MOUNT="/bin/mount"
	if [ "$(readlink ${MOUNT})" != "/bin/mount.util-linux" ]; then
		# Busybox mount. Clear default params
		MOUNT_PARAMS=""
		# Mount 'linux' partition as read-only
		if [ "${PARTNAME}" = "linux" ]; then
			MOUNT_PARAMS="${MOUNT_PARAMS} -r"
		fi
	fi
fi

# Create mount point if needed
MOUNTPOINT="/mnt/${PARTNAME}"
[ -d "${MOUNTPOINT}" ] || mkdir -p ${MOUNTPOINT}

if [ "${SUBSYSTEM}" = "block" ]; then
	if ! ${MOUNT} -t auto ${MOUNT_PARAMS} ${DEVNAME} ${MOUNTPOINT}; then
		logger -t udev "ERROR: Could not mount ${DEVNAME} under ${MOUNTPOINT}"
		rmdir --ignore-fail-on-non-empty ${MOUNTPOINT}
	fi
elif [ "${SUBSYSTEM}" = "mtd" ]; then
	# Before attaching, find out if partition already attached
	MTD_NUM="$(echo ${MTDN} | sed -ne 's,.*mtd\([0-9]\+\),\1,g;T;p')"
	for ubidev in /sys/devices/virtual/ubi/*; do
		echo "${ubidev}" | grep -qs '/sys/devices/virtual/ubi/\*' && continue
		mtd_att="$(cat ${ubidev}/mtd_num)"
		if [ "${mtd_att}" = "${MTD_NUM}" ]; then
			dev_number="$(echo ${ubidev} | sed -ne 's,.*ubi\([0-9]\+\),\1,g;T;p')"
		fi
	done

	# If not already attached, attach and get UBI device number
	if [ -z "${dev_number}" ]; then
		dev_number="$(ubiattach -p ${DEVNAME} 2>/dev/null | sed -ne 's,.*device number \([0-9]\).*,\1,g;T;p' 2>/dev/null)"
	fi
	# Check if volume exists.
	if ubinfo /dev/ubi${dev_number} -N ${PARTNAME} >/dev/null 2>&1; then
		# Mount the volume.
		if ! mount -t ubifs ubi${dev_number}:${PARTNAME} ${MOUNT_PARAMS} ${MOUNTPOINT}; then
			logger -t udev "ERROR: Could not mount '${PARTNAME}' partition"
			rmdir --ignore-fail-on-non-empty ${MOUNTPOINT}
		fi
	else
		logger -t udev "ERROR: Could not mount '${PARTNAME}' partition, volume not found"
		rmdir --ignore-fail-on-non-empty ${MOUNTPOINT}
	fi
elif [ "${SUBSYSTEM}" = "ubi" ]; then
	# In the case of a 'system' partition with many UBI volumes, the device
	# is always /dev/ubi0
	# Mount the volume.
	if ! mount -t ubifs ubi0:${PARTNAME} ${MOUNT_PARAMS} ${MOUNTPOINT}; then
		logger -t udev "ERROR: Could not mount '${PARTNAME}' volume"
		rmdir --ignore-fail-on-non-empty ${MOUNTPOINT}
	fi
fi
