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
UBOOT_REDUNDANT="$4"
UBOOT_FILE="/tmp/${UBOOT_NAME}"
UBOOT_BLOCK_MAIN="mmcblk0boot0"
UBOOT_BLOCK_REDUNDANT="mmcblk0boot1"
UBOOT_MMC_DEV_MAIN="/dev/${UBOOT_BLOCK_MAIN}"
UBOOT_MMC_DUMP="/tmp/u-boot-dump.hex"
UBOOT_ENCRYPTED_DEK="/tmp/u-boot-encrypted-with-dek.imx"

DEK_FILE="/tmp/dek.bin"
DEK_KEY_SIZE="32"
DEK_BLOB_SIZE="$((DEK_KEY_SIZE + 56))" # DEK blob has an overhead of 56 bytes: header (8 bytes) + random AES-256 key (32 bytes)+ MAC (16 bytes).
DEK_BLOB_HEADER_CCIMX6="8100584"
DEK_BLOB_HEADER_CCIMX8M="8100484"
DEK_BLOB_HEADER_CCIMX8X="00580081"

PLATFORM="$(tr -d '\0' </proc/device-tree/digi,machine,name)"
if expr "${PLATFORM}" : "ccimx8m.*" >/dev/null; then
	# CCIMX8M platforms have a hardcoded DEK blob size of 96 bytes.
	DEK_BLOB_SIZE="96"
fi
if expr "${PLATFORM}" : "ccimx8x.*" >/dev/null; then
	# The U-Boot seek variable depends on the hardware variant of the i.MX8X module.
	SOC_REV="$(fw_printenv -n soc_rev)"
	if [ -n "${SOC_REV}" ]; then
		case "${SOC_REV}" in
			A0)
				UBOOT_SEEK_KB="33"
			;;
			B0)
				UBOOT_SEEK_KB="32"
			;;
			*)
				UBOOT_SEEK_KB="0"
			;;
		esac
	fi
fi

clean_artifacts ()
{
	rm -f "${DEK_FILE}" "${UBOOT_MMC_DUMP}" "${UBOOT_ENCRYPTED_DEK}"
}

exit_error ()
{
	local ERROR_MESSAGE="$1"
	local ERROR_CODE="$2"

	echo "${ERROR_MESSAGE}"
	clean_artifacts
	[ -z "${ERROR_CODE}" ] && exit 1 || exit "${ERROR_CODE}"
}

dump_dek_ccimx8x ()
{
	local AHAB_AUTH_CONTAINER_TAG="87"
	local AHAB_AUTH_SIG_BLOCK_TAG="90"
	local AHAB_VERSION="00"
	local CONT_HEADER_OFFSET="0x400"

	# Dump U-Boot first 100Kb to file. The second AHAB container, which contains the DEK blob is there.
	dd if="${UBOOT_MMC_DEV_MAIN}" of="${UBOOT_MMC_DUMP}" count=100 bs=1K skip="${UBOOT_SEEK_KB}" 2>/dev/null
	# Look for second AHAB authentication container. Second Container Header is set with a 1KB padding (0x400)
	local AUTH_CONTAINER_TAG="$(hexdump -C "${UBOOT_MMC_DUMP}" -s "${CONT_HEADER_OFFSET}" | grep -m 1 "${AHAB_AUTH_CONTAINER_TAG}" | awk '{print $2 $5}')"
	if [ "${AUTH_CONTAINER_TAG}" = "${AHAB_VERSION}${AHAB_AUTH_CONTAINER_TAG}" ]; then
		# Determine second signature block relative and final offset.
		local SECOND_SIG_BLOCK_OFFSET="0x$(hexdump -C -s "${CONT_HEADER_OFFSET}" "${UBOOT_MMC_DUMP}" | grep -m 1 "${AHAB_AUTH_CONTAINER_TAG}" | awk '{print $15 $14}')"
		local SECOND_SIG_BLOCK="$((CONT_HEADER_OFFSET + SECOND_SIG_BLOCK_OFFSET))"
		# Validate second signature block.
		local AUTH_SIG_BLOCK_TAG="$(hexdump -C -s "${SECOND_SIG_BLOCK}" "${UBOOT_MMC_DUMP}" | grep -m 1 "${AHAB_AUTH_SIG_BLOCK_TAG}" | awk '{print $2 $5}')"
		if [ "${AUTH_SIG_BLOCK_TAG}" = "${AHAB_VERSION}${AHAB_AUTH_SIG_BLOCK_TAG}" ]; then
			# Determine DEK blob relative and final offset.
			local DEK_BLOB_RELATIVE_OFFSET="0x$(hexdump -C -s "${SECOND_SIG_BLOCK}" "${UBOOT_MMC_DUMP}" | grep -m 1 "${AHAB_AUTH_SIG_BLOCK_TAG}" | awk '{print $13 $12}')"
			DEK_AHAB_OFFSET="$((SECOND_SIG_BLOCK + DEK_BLOB_RELATIVE_OFFSET))"
			# Dump DEK blob into to a file.
			dd if="${UBOOT_MMC_DUMP}" of="${DEK_FILE}" count="${DEK_BLOB_SIZE}" bs=1 skip="${DEK_AHAB_OFFSET}" 2>/dev/null
			local rc=$?
			if [ "${rc}" -ne 0 ]; then
				exit_error "## ERROR: DEK dump to file failed." "${rc}"
			fi
			# Validate DEK blob.
			if ! dd if="${DEK_FILE}" bs=1 count=4 2>/dev/null | hexdump -ve '1/1 "%.2X"' | grep -q "${DEK_BLOB_HEADER_CCIMX8X}"; then
				exit_error "## ERROR: Could not find DEK blob."
			fi
		else
			exit_error "## ERROR: AHAB authentication signature block tag not found."
		fi
	else
		exit_error "## ERROR: AHAB authentication container tag not found."
	fi
}

dump_dek_ccimx8m_ccimx6 ()
{
	local IVT_HEADER="d1 00 20 4"  # The last byte lacks one digit on purpose, to match 40, 41 and 42; all HAB versions.
	local DEK_BLOB_HEADER="$1"

	# Look for U-Boot in the eMMC. It starts with the IVT table header.
	dd if="${UBOOT_MMC_DEV_MAIN}" of="${UBOOT_MMC_DUMP}" bs=1k skip="${UBOOT_SEEK_KB}" 2>/dev/null
	local UBOOT_START="0x$(hexdump -C "${UBOOT_MMC_DUMP}" | grep -m 1 "${IVT_HEADER}" | head -1 | cut -c -8)"
	if [ "${UBOOT_START}" = "0x" ]; then
		exit_error "## ERROR: Could not find U-Boot on MMC."
	fi
	# DEK blob must be extracted from the SPL image. First determine SPL image size.
	local SPL_SIZE_OFFSET="$((UBOOT_START + 36))"  # Size information is at offset 36.
	local SPL_SIZE="$(hexdump -n 4 -s "${SPL_SIZE_OFFSET}" -e '/4 "%d\t" "\n"' "${UBOOT_MMC_DUMP}")"
	# Determine SPL size in blocks of 1Kb. Round up division if it is not exact.
	local DUMP_SIZE="$(( (SPL_SIZE + 1023) / 1024 ))"
	# Dump SPL image to file.
	dd if="${UBOOT_MMC_DEV_MAIN}" of="${UBOOT_MMC_DUMP}" bs=1k count="${DUMP_SIZE}" skip="${UBOOT_SEEK_KB}" conv=fsync 2>/dev/null
	# Look for the DEK blob in SPL image.
	DEK_SPL_OFFSET="$(hexdump -ve '1/1 "%.2X"' ${UBOOT_MMC_DUMP} | awk -v pattern="${DEK_BLOB_HEADER}" 'BEGIN{IGNORECASE=1} {pos=index($0, pattern)} pos {print (pos-1)/2}')"
	# Dump the DEK blob to file.
	dd if="${UBOOT_MMC_DUMP}" of="${DEK_FILE}" count="${DEK_BLOB_SIZE}" bs=1 skip="${DEK_SPL_OFFSET}" 2>/dev/null
	local rc=$?
	if [ "${rc}" -ne 0 ]; then
		exit_error "## ERROR: DEK dump to file failed." "${rc}"
	fi
	# Validate DEK blob.
	if ! dd if="${DEK_FILE}" bs=1 count=4 2>/dev/null | hexdump -ve '1/1 "%.2X"' | grep -q "${DEK_BLOB_HEADER}"; then
		exit_error "## ERROR: Could not find DEK blob."
	fi
}

dump_dek_ccimx93 ()
{
	exit_error "## ERROR: DEK support not implemented yet for CCIMX93."
}

dump_dek ()
{
	case "${PLATFORM}" in
		ccimx8x*)
			dump_dek_ccimx8x
		;;
		ccimx8m*)
			dump_dek_ccimx8m_ccimx6 "${DEK_BLOB_HEADER_CCIMX8M}"
		;;
		ccimx6*)
			dump_dek_ccimx8m_ccimx6 "${DEK_BLOB_HEADER_CCIMX6}"
		;;
		ccimx93*)
			dump_dek_ccimx93
		;;
		*)
			exit_error "## ERROR: Device not supported ${PLATFORM}."
		;;
	esac
}

append_dek_ccimx8x ()
{
	cp "${UBOOT_FILE}" "${UBOOT_ENCRYPTED_DEK}"
	# Insert the DEK blob into the AHAB container.
	dd if="${DEK_FILE}" of="${UBOOT_ENCRYPTED_DEK}" bs=1 seek="${DEK_AHAB_OFFSET}" conv=notrunc 2>/dev/null
	local rc=$?
	if [ "${rc}" -ne 0 ]; then
		exit_error "## ERROR: Merging DEK with AHAB container failed." "${rc}"
	fi
}

append_dek_ccimx8m ()
{
	cp "${UBOOT_FILE}" "${UBOOT_ENCRYPTED_DEK}"
	# Insert the DEK blob into the SPL image.
	dd if="${DEK_FILE}" of="${UBOOT_ENCRYPTED_DEK}" bs=1 seek="${DEK_SPL_OFFSET}" conv=notrunc 2>/dev/null
	local rc=$?
	if [ "${rc}" -ne 0 ]; then
		exit_error "## ERROR: Merging DEK with SPL image failed." "${rc}"
	fi
	# Get total iMX-Boot file size.
	local UBOOT_FILE_SIZE="$(stat -L -c %s "${UBOOT_FILE}")"
	# Determine FIT DEK blob offset.
	local DEK_FIT_OFFSET="$((UBOOT_FILE_SIZE - DEK_BLOB_SIZE))"
	# Insert the DEK blob into the FIT image.
	dd if="${DEK_FILE}" of="${UBOOT_ENCRYPTED_DEK}" bs=1 seek="${DEK_FIT_OFFSET}" conv=notrunc 2>/dev/null
	local rc=$?
	if [ "${rc}" -ne 0 ]; then
		exit_error "## ERROR: Merging DEK with FIT image failed." "${rc}"
	fi
}

append_dek_ccimx6 ()
{
	cat "${UBOOT_FILE}" "${DEK_FILE}" > "${UBOOT_ENCRYPTED_DEK}"
	local rc=$?
	if [ "${rc}" -ne 0 ]; then
		exit_error "## ERROR: Merging DEK with U-Boot image failed." "${rc}"
	fi
}

append_dek_ccimx93 ()
{
	exit_error "## ERROR: DEK support not implemented yet for CCIMX93."
}

append_dek ()
{
	dump_dek
	case "${PLATFORM}" in
		ccimx8x*)
			append_dek_ccimx8x
		;;
		ccimx8m*)
			append_dek_ccimx8m
		;;
		ccimx6*)
			append_dek_ccimx6
		;;
		ccimx93*)
			append_dek_ccimx93
		;;
		*)
			exit_error "## ERROR: Device not supported: ${PLATFORM}."
		;;
	esac
	UBOOT_FILE="${UBOOT_ENCRYPTED_DEK}"
}

write_uboot_emmc ()
{
	local UBOOT_BLOCK="$1"

	# Enable write access in the U-Boot partition.
	echo 0 > "/sys/block/${UBOOT_BLOCK}/force_ro"
	# Write the U-Boot into the eMMC.
	dd if="${UBOOT_FILE}" of="/dev/${UBOOT_BLOCK}" seek="${UBOOT_SEEK_KB}" bs=1K 2>/dev/null
	local rc=$?
	# Disable write access in U-Boot partition.
	echo 1 > "/sys/block/${UBOOT_BLOCK}/force_ro"
	# Check update operation result.
	if [ "${rc}" -ne 0 ]; then
		exit_error "## ERROR: failed to write file ${UBOOT_FILE} to /dev/${UBOOT_BLOCK}" "${rc}"
	fi
	echo "U-Boot successfully writen to /dev/${UBOOT_BLOCK}"
}

# If U-Boot is encrypted, the DEK key blob needs to be extracted from existing U-Boot
# and appended to the new U-Boot before writing it.
if [ "${UBOOT_ENC}" = "enc" ]; then
	append_dek
fi
# Write U-Boot
write_uboot_emmc ${UBOOT_BLOCK_MAIN}
# Check if redundant U-Boot update is requested.
if [ "${UBOOT_REDUNDANT}" = "redundant" ]; then
	write_uboot_emmc ${UBOOT_BLOCK_REDUNDANT}
fi
# Clean intermediate artifacts.
clean_artifacts

exit 0
