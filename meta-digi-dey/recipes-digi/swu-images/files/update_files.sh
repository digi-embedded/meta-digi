#!/bin/sh
#===============================================================================
#
#  update_files
#
#  Copyright (C) 2023 by Digi International Inc.
#  All rights reserved.
#
#  This program is free software; you can redistribute it and/or modify it
#  under the terms of the GNU General Public License version 2 as published by
#  the Free Software Foundation.
#
#
#  !Description: SWU update files script
#
#===============================================================================

# Sanity check. This script should be always executed with at least one argument.
if [ $# -lt 1 ]; then
	exit 1;
fi

# Variables.
FS_TYPE="ext4"
LINUX_DEV_BLOCK="/dev/mmcblk0p1"
LINUX_MOUNT_POINT="/mnt/linux"
ROOTFS_DEV_BLOCK="/dev/mmcblk0p3"
ROOTFS_MOUNT_POINT="/system"

# Determines whether the file system type is UBI or not.
is_ubifs() {
	[ -c "/dev/ubi0" ]
}

# Retrieves the MTD partition number corresponding to the given partition name.
#
# Args:
#   $1: partition name.
#
# Returns:
#   The MTD partition number corresponding to the given partition name, -1 if
#   not found.
get_mtd_number() {
	local mtd_line="$(sed -ne "/${1}/s,^mtd\([0-9]\+\).*,\1,g;T;p" /proc/mtd)"
	echo "${mtd_line:--1}"
}

# Creates the UBI device for the given MTD partition number.
#
# Args:
#   $1: the MTD partition number to create the UBI device for.
#
# Returns:
#   The created UBI device number for the given MTD partition number, -1 if error.
create_ubi_device() {
	local dev_number="$(ubiattach -m "${1}" 2>/dev/null | sed -ne 's,.*device number \([0-9]\).*,\1,g;T;p' 2>/dev/null)"
	echo "${dev_number:--1}"
}

# Retrieves the UBI device number containing the given partition name. If the
# device does not exist, the method attempts to create it based on the MTD dev
# number containing the desired partition.
#
# Args:
#   $1: partition name.
#
# Returns:
#   The UBI device number containing the given partition name, -1 if not found.
get_ubi_device() {
	local ubi_devices="$(ubinfo | grep "Present UBI devices:" | cut -d ":" -f2 | xargs | sed -e 's/,//g')"
	for ubi_device in ${ubi_devices}; do
		if ubinfo "/dev/${ubi_device}" -a | grep -qe "Name:.*$1"; then
			echo "${ubi_device}" | tr -dc '0-9'
			return 0
		fi
	done

	# Look for the MTD number containing the given partition name.
	local mtd_num="$(get_mtd_number "${1}")"
	if [ "${mtd_num}" = "-1" ]; then
		echo "-1"
		return 1
	else
		# Create the UBI device.
		ubi_device_number="$(create_ubi_device "${mtd_num}")"
		echo "${ubi_device_number}"
	fi
}

# Mounts all required partitions to perform the firmware update based on the update
# running source and file system type.
mount_partitions() {
	# Determine whether the update is running from recovery partition or not.
	BOOT_RECOVERY="$(fw_printenv -n boot_recovery)"
	if [ "${BOOT_RECOVERY}" = "yes" ]; then
		# Update is running from recovery partition. We need to mount both,
		# the rootfs and the kernel partitions. To do so first determine the
		# filesystem type, assume it is MMC device.
		if is_ubifs; then
			FS_TYPE="ubifs"
			# Look for the UBI device containing 'linux' partition.
			local linux_ubi_device="$(get_ubi_device linux)"
			[ "${linux_ubi_device}" = "-1" ] && { echo "Unable to find UBI device containing 'linux' partition."; exit 1; }
			LINUX_DEV_BLOCK="ubi${linux_ubi_device}:linux"
			# Look for the UBI device containing 'rootfs' partition.
			local rootfs_ubi_device="$(get_ubi_device rootfs)"
			[ "${rootfs_ubi_device}" = "-1" ] && { echo "Unable to find UBI device containing 'rootfs' partition."; exit 1; }
			ROOTFS_DEV_BLOCK="ubi${rootfs_ubi_device}:rootfs"
		fi
		# Mount 'rootfs' partition.
		mkdir -p "${ROOTFS_MOUNT_POINT}"
		mount -t "${FS_TYPE}" "${ROOTFS_DEV_BLOCK}" "${ROOTFS_MOUNT_POINT}"
		# Mount 'linux' partition.
		LINUX_MOUNT_POINT="${ROOTFS_MOUNT_POINT}${LINUX_MOUNT_POINT}"
		mkdir -p "${LINUX_MOUNT_POINT}"
		if ! is_ubifs; then
			FS_TYPE="auto"
		fi
		mount -t "${FS_TYPE}" "${LINUX_DEV_BLOCK}" "${LINUX_MOUNT_POINT}"
	else
		# Update is running from the active system. In this case the 'rootfs' and 'linux'
		# partitions are already mounted; however 'linux' partition is in R/O mode. Just
		# remount 'linux' partition as R/W.
		mount -o remount,rw "${LINUX_MOUNT_POINT}"
	fi
}

# Called just before installation process starts.
if [ "${1}" = "preinst" ]; then
	mount_partitions

	# TODO: Execute custom code here. For example:
	# - Mount additional devices/partitions.
	# - Stop services/process before installing files.
fi

# Called just after installation process ends.
if [ "${1}" = "postinst" ]; then
	:

	# TODO: Execute custom code here. For example:
	# - Clean directories.
	# - Post-process files.
fi
