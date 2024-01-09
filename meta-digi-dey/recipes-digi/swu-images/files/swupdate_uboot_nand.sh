#!/bin/sh
#===============================================================================
#
#  Copyright (C) 2022-2024 by Digi International Inc.
#  All rights reserved.
#
#  This program is free software; you can redistribute it and/or modify it
#  under the terms of the GNU General Public License version 2 as published by
#  the Free Software Foundation.
#
#
#  Description:
#    Script will be called by swupdate to install a new u-boot within linux.
#===============================================================================

UBOOT_NAME="$1"
UBOOT_ENC="$2"
UBOOT_SEEK_KB="$3"
UBOOT_TFA_NAME="$4"
UBOOT_TFA_FILE="/tmp/${UBOOT_TFA_NAME}"
UBOOT_FILE="/tmp/${UBOOT_NAME}"
UBOOT_NAND_DUMP="/tmp/u-boot-dump.hex"
UBOOT_ENCRYPTED_DEK="/tmp/u-boot-encrypted-with-dek.imx"

DEK_FILE="/tmp/dek.bin"
DEK_KEY_SIZE="32"
DEK_BLOB_SIZE="$((DEK_KEY_SIZE + 56))"  # DEK blob has an overhead of 56 bytes: header (8 bytes) + random AES-256 key (32 bytes) + MAC (16 bytes).
DEK_BLOB_HEADER="8100584"  # The last byte lacks one digit on purpose, to match 40, 41 and 42; all HAB versions.

PLATFORM="$(tr -d '\0' </proc/device-tree/digi,machine,name)"

clean_artifacts ()
{
	rm -f "${DEK_FILE}" "${UBOOT_NAND_DUMP}" "${UBOOT_ENCRYPTED_DEK}"
}

exit_error ()
{
	local ERROR_MESSAGE="$1"
	local ERROR_CODE="$2"

	echo "${ERROR_MESSAGE}"
	clean_artifacts
	[ -z "${ERROR_CODE}" ] && exit 1 || exit "${ERROR_CODE}"
}

dump_dek_ccimx6ul ()
{
	local IVT_HEADER="d1 00 20 4"  # The last byte lacks one digit on purpose, to match 40, 41 and 42; all HAB versions.
	local UBOOT_MTD_DEV="/dev/mtd0"

	# Look for U-Boot in the NAND. It starts with the IVT table header.
	local UBOOT_START="0x$(hexdump -C "${UBOOT_MTD_DEV}" | grep -m 1 "${IVT_HEADER}" | head -1 | cut -c -8)"
	if [ "${UBOOT_START}" = "0x" ]; then
		exit_error "## ERROR: Could not find U-Boot on NAND."
	fi
	# DEK blob is located at the end of the image. Determine the total size of the image.
	local DUMP_SIZE_OFFSET="$((UBOOT_START + 36))"  # Size is located at an offset of 36 (32 bytes of IVT header + 4 bytes of image destination).
	local DUMP_SIZE="$(hexdump -n 4 -s "${DUMP_SIZE_OFFSET}" -e '/4 "0x%08x" "\n"' "${UBOOT_MTD_DEV}")"
	# The dump start needs to be aligned (U-Boot always leaves 0x400 for DOS table).
	local DUMP_START=$((UBOOT_START - 0x400))
	# Read the complete image (to skip alignment issues) and keep only the DEK blob (which is at the end).
	nanddump -s "${DUMP_START}" -l "${DUMP_SIZE}" "${UBOOT_MTD_DEV}" | tail -c "${DEK_BLOB_SIZE}" > "${DEK_FILE}"
	local rc=$?
	if [ "${rc}" -ne 0 ]; then
		exit_error "## ERROR: DEK dump to file failed." "${rc}"
	fi
	# Validate the DEK blob.
	if ! dd if="${DEK_FILE}" bs=1 count=4 2>/dev/null | hexdump -ve '1/1 "%.2X"' | grep -q "${DEK_BLOB_HEADER}"; then
		exit_error "## ERROR: Could not find DEK blob."
	fi
}

dump_dek ()
{
	case "${PLATFORM}" in
		ccimx6ul*)
			dump_dek_ccimx6ul
		;;
		*)
			exit_error "## ERROR: Device not supported ${PLATFORM}."
		;;
	esac
}

append_dek_ccimx6ul ()
{
	cat "${UBOOT_FILE}" "${DEK_FILE}" > "${UBOOT_ENCRYPTED_DEK}"
	local rc=$?
	if [ "${rc}" -ne 0 ]; then
		exit_error "## ERROR: Merging DEK with U-Boot image failed." "${rc}"
	fi
}

append_dek ()
{
	dump_dek
	case "${PLATFORM}" in
		ccimx6ul*)
			append_dek_ccimx6ul
		;;
		*)
			exit_error "## ERROR: Device not supported: ${PLATFORM}."
		;;
	esac
	UBOOT_FILE="${UBOOT_ENCRYPTED_DEK}"
}

write_file_to_nand ()
{
	local FLASH_DEV="$1"
	local FW_FILE="$2"

	# Sanity check.
	if [ ! -c "${FLASH_DEV}" ]; then
		exit_error "## ERROR: Invalid MTD partition: ${FLASH_DEV}."
	fi
	# Clean MTD partition.
	flash_eraseall "${FLASH_DEV}"
	local rc=$?
	if [ "${rc}" -ne 0 ]; then
		exit_error "## ERROR: Could not erase ${FLASH_DEV} partition." "${rc}"
	fi
	# Write file to NAND.
	nandwrite -p "${FLASH_DEV}" "${FW_FILE}"
	local rc=$?
	if [ "${rc}" -ne 0 ]; then
		exit_error "## ERROR: Could not write file to NAND." "${rc}"
	fi
}

get_mtd_number_from_partition ()
{
	local PARTITION_NAME="$1"
	local MTD_NUM="$(sed -ne "/${PARTITION_NAME}/s,^mtd\([0-9]\+\).*,\1,g;T;p" /proc/mtd)"

	echo "${MTD_NUM}"
}

# If U-Boot is encrypted, the DEK key blob needs to be extracted from existing U-Boot
# and appended to the new U-Boot before writing it.
if [ "${UBOOT_ENC}" = "enc" ]; then
	append_dek
fi
# Write U-Boot
if expr "${PLATFORM}" : "ccmp1.*" >/dev/null; then
	# Install TFA file in fsbl1 partition.
	write_file_to_nand "/dev/mtd$(get_mtd_number_from_partition fsbl1)" "${UBOOT_TFA_FILE}"
	# Install U-Boot FIP file in fip-a partition.
	write_file_to_nand "/dev/mtd$(get_mtd_number_from_partition fip-a)" "${UBOOT_FILE}"
else
	# Mount debug file system to remove some kobs-ng warnings.
	if ! grep -qs debugfs /proc/mounts; then
		mount -t debugfs debugfs /sys/kernel/debug/
	fi
	# Install U-Boot onto the Nand Flash using kobs-ng.
	kobs-ng init -x -v "${UBOOT_FILE}"
	rc=$?
	if [ "${rc}" -ne 0 ]; then
		exit_error "## ERROR: Could not write file to NAND." "${rc}"
	fi
fi
# Clean intermediate artifacts.
clean_artifacts

exit 0
