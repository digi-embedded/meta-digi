#!/bin/sh
#===============================================================================
#
#  update_rdiff
#
#  Copyright (C) 2023 by Digi International Inc.
#  All rights reserved.
#
#  This program is free software; you can redistribute it and/or modify it
#  under the terms of the GNU General Public License version 2 as published by
#  the Free Software Foundation.
#
#
#  !Description: SWU update rdiff script
#
#===============================================================================

# Sanity check. This script should be always executed with at least one argument.
if [ $# -lt 1 ]; then
	exit 1;
fi

# Sanity check. Do not run in single-boot systems.
[ "$(fw_printenv -n dualboot)" = "yes" ] || { echo "This update cannot be applied to single-boot systems, aborting..."; exit 1; }

# Sanity check. Do not run in R/W systems.
if ! grep "/proc/mounts" -qe "squashfs"; then
	echo "This update cannot be applied to R/W systems, aborting..."
	exit 1
fi

# Variables.
BLOCK_SIZE=4096
ROOTFS_NAME="rootfs"
ROOTFS_SOURCE_ENDPOINT="/dev/rdiff_source_rootfs"
ROOTFS_DEV_BLOCK="mmcblk0p3"
ROOTFS_DEV_BLOCK_A="mmcblk0p3"
ROOTFS_DEV_BLOCK_B="mmcblk0p4"

# Determines whether the file system type is UBIFS or not.
is_ubifs() {
	[ -c "/dev/ubi0" ]
}

# Determines whether the system is dualboot or not.
is_dualboot() {
	[ "$(fw_printenv -n dualboot)" = "yes" ]
}

# Retrieves the dualboot active system letter.
#
# Returns:
#   The dualboot active system letter: 'a' for primary, 'b' for secondary.
get_active_system() {
	local active_system="$(fw_printenv -n active_system)"
	echo "${active_system}" | cut -d_ -f2
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

# Retrieves the UBI volume containing the given partition name.
#
# Args:
#   $1: partition name.
#
# Returns:
#   The UBI volume containing the given partition name, -1 if not found.
get_ubi_volume() {
	# Look for the UBI device containing given partition.
	local ubi_device="$(get_ubi_device "${1}")"
	if [ "${ubi_device}" = "-1" ]; then
		echo "-1"
		return 1
	fi
	# Look for the UBI volume containing given partition.
	local ubi_volume="$(ubinfo -d "${ubi_device}" -N "${1}" | grep "Volume ID" | cut -d ":" -f2 | xargs | cut -d " " -f1)"
	if [ -z "${ubi_volume}" ]; then
		echo "-1"
		return 1
	fi
	echo "ubi${ubi_device}_${ubi_volume}"
}

# Creates the 'rootfs' source endpoint.
#
# Update source for the 'rootfs' partition cannot be determined at build time. For MTD
# devices, it depends on whether system is based on single or multiple MTD partitions.
# For this reason, hook the source update to a well known endpoint and just create the
# required link from the running system once all the information is available.
create_source_endpoint() {
	# Initialize vars. Assume system is MMC based.
	local rootfs_source_partiton="${ROOTFS_NAME}"
	local rootfs_source_dev="${ROOTFS_DEV_BLOCK}"

	# Remove previous link.
	[ -L "${ROOTFS_SOURCE_ENDPOINT}" ] && unlink "${ROOTFS_SOURCE_ENDPOINT}"

	# Update variables for dualboot systems.
	if is_dualboot; then
		local active_part="$(get_active_system)"
		rootfs_source_partiton="${rootfs_source_partiton}_${active_part}"
		if [ "${active_part}" = "a" ]; then
			rootfs_source_dev=${ROOTFS_DEV_BLOCK_A}
		else
			rootfs_source_dev=${ROOTFS_DEV_BLOCK_B}
		fi
	fi

	# Update variables for MTD systems.
	if is_ubifs; then
		# Look for 'rootfs' source UBI volume.
		rootfs_source_dev="$(get_ubi_volume "${rootfs_source_partiton}")"
		[ "${rootfs_source_dev}" = "-1" ] && { echo "Unable to find UBI volume containing '${rootfs_source_partiton}' partition."; exit 1; }
	fi

	# Create link.
	ln -s "${rootfs_source_dev}" "${ROOTFS_SOURCE_ENDPOINT}"
}

# Validates the base image before applying the RDIFF patch.
#
# Args:
#   $1: Checksum of original base image.
validate_base_image() {
	local fs_size="$(hexdump -s 0x28 -n 4 -e '1/4 "%d"' "${ROOTFS_SOURCE_ENDPOINT}")"
	fs_size=$(( (fs_size + 0xfff) & 0xfffff000 ))
	local n_blocks=$(( fs_size/BLOCK_SIZE ))
	local checksum="$(dd if="${ROOTFS_SOURCE_ENDPOINT}" bs="${BLOCK_SIZE}" count="${n_blocks}" 2> /dev/null | sha256sum | cut -d " " -f1)"
	
	if [ "${checksum}" != "${1}" ]; then		
		echo "[ERROR] Base image is not the expected one or has been modified. Aborting update..."
		exit 1
	fi
}

# Called just before installation process starts.
if [ "${1}" = "preinst" ]; then
	create_source_endpoint
	validate_base_image "${2}"

	# TODO: Execute custom code here. For example:
	# - Mount additional devices/partitions.
	# - Stop services/process before installation.
fi

# Called just after installation process ends.
if [ "${1}" = "postinst" ]; then
	:

	# TODO: Execute custom code here. For example:
	# - Clean files/directories.
	# - Post-process files.
fi
