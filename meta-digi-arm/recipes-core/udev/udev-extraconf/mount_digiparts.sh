#!/bin/sh
#===============================================================================
#
#  mount_bootparts.sh
#
#  Copyright (C) 2014-2023 by Digi International Inc.
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
BASE_INIT_ORIG="$(readlink -f "@base_sbindir@/init.orig")"
INIT_SYSTEMD="@systemd_unitdir@/systemd"
EMMCROOTFS="$(grep -qs 'root=PARTUUID.*' /proc/cmdline 2>/dev/null && echo 1)"

get_active_system() {
	if [ -z "${EMMCROOTFS}" ]; then
		# For a read-only filesystem this will be /dev/ubiblock0_X
		# For an ubifs filesystem this will be ubiX:rootfs_X
		ACTIVE_SYSTEM="$(sed -e 's/^.*root=\([^ ]*\) .*$/\1/' /proc/cmdline 2>/dev/null)"
		if ! echo "${ACTIVE_SYSTEM}" | grep -qs rootfs; then
			# From /dev/ubiblock0_X to /dev/ubi0_X
			ACTIVE_SYSTEM="/dev/ubi${ACTIVE_SYSTEM#/dev/ubiblock}"
			#Volume ID:   5 (on ubi0)
			#Type:        dynamic
			#Alignment:   1
			#Size:        1817 LEBs (230715392 bytes, 220.0 MiB)
			#State:       OK
			#Name:        rootfs_b
			#Character device major/minor: 242:6
			ACTIVE_SYSTEM="$(ubinfo "${ACTIVE_SYSTEM}" | sed -ne '/^Name/s,.* \([^[:blank:]]\+\)$,\1,g;T;p')"
		fi
	else
		local MMCROOT_DEV

		MMCROOT_DEV="$(stat -c%D /)"

		for label in /dev/disk/by-partlabel/*; do
			if [ "$(stat -c"%02t%02T" "$(realpath "${label}")")" = "${MMCROOT_DEV}" ]; then
				ACTIVE_SYSTEM="$(basename "${label}")"
				break
			fi
		done
	fi

	if [ -z "${ACTIVE_SYSTEM}" ]; then
		echo "[ERROR] Unable to get active system."
		return 1
	fi

	ACTIVE_SYSTEM="${ACTIVE_SYSTEM##*_}"

	return 0
}

if [ "${SUBSYSTEM}" = "block" ]; then
	PARTNAME="${ID_PART_ENTRY_NAME}"
elif [ "${SUBSYSTEM}" = "mtd" ]; then
	MTDN="$(echo ${DEVNAME} | cut -f 3 -d /)"
	PARTNAME="$(grep ${MTDN} /proc/mtd | sed -ne 's,.*"\(.*\)",\1,g;T;p')"
elif [ "${SUBSYSTEM}" = "ubi" ]; then
	PARTNAME="$(cat /sys/${DEVPATH}/name)"
	# Multi-MTD systems only have one UBI volume per MTD partition that is
	# called the same as the MTD partition. Do nothing for UBI events if the
	# MTD partition is called the same, as they are already handled by the
	# "mtd" subsystem rule
	result="$(grep \"${PARTNAME}\"$ /proc/mtd)"
	[ -n "${result}" ] && exit 0
fi

MOUNT_FOLDER=${PARTNAME}
MOUNT_PARAMS="-o silent"
# Mount 'linux' partition as read-only
if [ "${PARTNAME}" = "linux" ] || [ "${PARTNAME}" = "linux_a" ] || [ "${PARTNAME}" = "linux_b" ]; then
	MOUNT_FOLDER="linux"
	MOUNT_PARAMS="${MOUNT_PARAMS} -o ro"
fi
MOUNTPOINT="/mnt/${MOUNT_FOLDER}"

# Skip if partition is already mounted. For example R/O systems with the '/etc' overlay enabled mount the 'data' partition in very early stages.
if grep -qs "${MOUNTPOINT}" /proc/mounts; then
	logger "Partition '${PARTNAME}' is already mounted, skipping..."
	exit 0
fi

# Get from proc/cmdline the active system "a" or "b"
get_active_system || exit

if [ "${PARTNAME}" = "linux_a" ] || [ "${PARTNAME}" = "linux_b" ]; then
	PARTINDEX="${PARTNAME#linux_}"
	if [ "${ACTIVE_SYSTEM}" != "${PARTINDEX}" ]; then
		logger "Skip mount partition '${PARTNAME}', because it is not the active system"
		exit 0
	fi
fi

# R/O systems using 'systemd' and '/etc' overlayfs do not link '/sbin/init' to 'systemd'. In these cases
# 'init' is renamed to 'init.orig' and that is the linked file, so check this case too.
if [ "x$BASE_INIT" = "x$INIT_SYSTEMD" ] || [ "x$BASE_INIT_ORIG" = "x$INIT_SYSTEMD" ]; then
	# systemd as init uses systemd-mount to mount block devices

	# Verify if unit is already launched, if so just restart it.
	if systemctl | grep -q "mnt-${PARTNAME}.mount"; then
		if ! systemctl restart "mnt-${PARTNAME}.mount"; then
			logger -t udev "ERROR: Could not mount '${DEVNAME}'"
			exit 1
		fi
		exit 0
	fi

	MOUNT="/usr/bin/systemd-mount"
	MOUNT_PARAMS="${MOUNT_PARAMS} --no-block"

	if [ -x "$MOUNT" ]; then
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
		if [ "${PARTNAME}" = "linux" ] || [ "${PARTNAME}" = "linux_a" ] || [ "${PARTNAME}" = "linux_b" ]; then
			MOUNT_PARAMS="${MOUNT_PARAMS} -r"
		fi
	fi
fi

# Create mount point if needed
[ -d "${MOUNTPOINT}" ] || mkdir -p ${MOUNTPOINT}

if [ "${SUBSYSTEM}" = "block" ]; then
	if ! ${MOUNT} -t auto ${MOUNT_PARAMS} ${DEVNAME} ${MOUNTPOINT}; then
		logger -t udev "ERROR: Could not mount ${DEVNAME} under ${MOUNTPOINT}"
		rmdir --ignore-fail-on-non-empty ${MOUNTPOINT}
	fi
elif [ "${SUBSYSTEM}" = "mtd" ]; then
	# Before attaching, find out if partition already attached
	MTD_NUM="$(echo ${MTDN} | sed -ne 's,.*mtd\([0-9]\+\),\1,g;T;p')"
	for ubidev in /sys/class/ubi/*; do
		echo "${ubidev}" | grep -qs '/sys/class/ubi/\*' && continue
		mtd_att="$(cat ${ubidev}/mtd_num)"
		if [ "${mtd_att}" = "${MTD_NUM}" ]; then
			dev_number="$(echo ${ubidev} | sed -ne 's,.*ubi\([0-9]\+\),\1,g;T;p')"
			break
		fi
	done

	# If not already attached, attach and get UBI device number
	if [ -z "${dev_number}" ]; then
		dev_number="$(ubiattach -p ${DEVNAME} 2>/dev/null | sed -ne 's,.*device number \([0-9]\).*,\1,g;T;p' 2>/dev/null)"
	fi
	# Check if volume exists.
	if ubinfo /dev/ubi${dev_number} -N ${PARTNAME} >/dev/null 2>&1; then
		# Mount the volume.
		if ! ${MOUNT} -t ubifs ubi${dev_number}:${PARTNAME} ${MOUNT_PARAMS} ${MOUNTPOINT}; then
			logger -t udev "ERROR: Could not mount '${PARTNAME}' partition"
			rmdir --ignore-fail-on-non-empty ${MOUNTPOINT}
		fi
	else
		logger -t udev "ERROR: Could not mount '${PARTNAME}' partition, volume not found"
		rmdir --ignore-fail-on-non-empty ${MOUNTPOINT}
	fi
elif [ "${SUBSYSTEM}" = "ubi" ]; then
	# Mount the volume.
	if ! ${MOUNT} -t ubifs ${DEVNAME} ${MOUNT_PARAMS} ${MOUNTPOINT}; then
		logger -t udev "ERROR: Could not mount '${PARTNAME}' volume"
		rmdir --ignore-fail-on-non-empty ${MOUNTPOINT}
	fi
fi
