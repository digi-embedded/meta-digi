#!/bin/sh
#===============================================================================
#
#  Copyright (C) 2022 by Digi International Inc.
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

UBOOT_FILE="$1"
UBOOT_ENC="$2"
uboot_seek_kb="$3"

echo "**** Start U-Boot update process *****"

PLATFORM="$(cat /proc/device-tree/digi,machine,name)"
UBOOT_MMC_DEV="/dev/mmcblk0boot0"

dump_dek ()
{
	OUTPUT_FILE="/tmp/dek.bin"
	KEY_SIZE_BYTES="32"
	ENCRYPTED_UBOOT_DEK="u-boot-encrypted-with-dek.imx"
	UBOOT_MMC_DUMP="/tmp/u-boot-dump.hex"

	# ConnectCore 8X
	if [ "${PLATFORM}" = "ccimx8x-sbc-pro" ] || [ "${PLATFORM}" = "ccimx8x-sbc-express" ]; then
		AHAB_AUTH_CONTAINER_TAG="87"
		AHAB_AUTH_SIG_BLOCK_TAG="90"
		AHAB_AUTH_BLOB_TAG="00 58 00 81"
		AHAB_VERSION="00"
		CONT_HEADER_OFFSET="0x400"

		dd if=${UBOOT_MMC_DEV} of=${UBOOT_MMC_DUMP} count=100 bs=1K skip=${uboot_seek_kb} 2>/dev/null
		auth_container_tag=$(hexdump -C "${UBOOT_MMC_DUMP}" | grep -m 1 "${AHAB_AUTH_CONTAINER_TAG}" | awk '{print $2 $5}')
		echo "auth_container_tag ${auth_container_tag}"
		if [ "${auth_container_tag}" = "${AHAB_VERSION}${AHAB_AUTH_CONTAINER_TAG}" ]; then
			sig_block_offset="0x$(hexdump -C "${UBOOT_MMC_DUMP}" | grep -m 1 "${AHAB_AUTH_CONTAINER_TAG}" | awk '{print $13 $14}')"
			echo " ++++ signature block offset ${sig_block_offset} "

			nd_sig_block="$((CONT_HEADER_OFFSET + sig_block_offset))"
			printf '++++ header offset 0x%x\n' ${nd_sig_block}
			auth_sig_block_tag=$(hexdump -C -s "${nd_sig_block}" "${UBOOT_MMC_DUMP}" | grep -m 1 "${AHAB_AUTH_SIG_BLOCK_TAG}" | awk '{print $2 $5}')
			echo "auth_sig_block_tag ${auth_sig_block_tag}"
			if [ "${auth_sig_block_tag}" = "${AHAB_VERSION}${AHAB_AUTH_SIG_BLOCK_TAG}" ]; then
				blob_offset="0x$(hexdump -C -s "${nd_sig_block}" "${UBOOT_MMC_DUMP}" -n 16 | awk '{print $13 $12}')"
				printf " ++++ blob offset 0x%x\n" ${blob_offset}
				dek_blob="$((nd_sig_block + blob_offset))"
				printf " ++++ dek_blob offset 0x%x\n" ${dek_blob}

				# DEK blobs have an overhead of 56 bytes.
				dek_blob_size=$((KEY_SIZE_BYTES + 56))

				# Dump dek blob into to a file
				dd of=${OUTPUT_FILE} if=${UBOOT_MMC_DUMP} count=${dek_blob_size} bs=1 skip=${dek_blob} 2>/dev/null
				rc=$?
				if [ $rc -ne 0 ]; then
					echo "DEK dump to the output file failed."
					return $rc
				fi
				echo "dump_dek: output file has been created."
				# Validate DEK blob
				if [ -z "$(dd if=${OUTPUT_FILE} bs=1 count=4 2>/dev/null | hexdump -C | grep "${AHAB_AUTH_BLOB_TAG}")" ]; then
					echo "Could not find DEK blob"
					rm -rf ${OUTPUT_FILE}
					return 1
				fi
				echo "DEK blob correctly dumped"
			else
				echo "## ERROR: AHAB authentication signature block tag not found."
			fi
		else
			echo "## ERROR: AHAB authentication container tag not found."
			return 1
		fi
	else
		#(The last byte lacks one digit on purpose, to match 40, 41 and 42; all HAB versions)
		UBOOT_HEADER="d1 00 20 4"
		if [ "${PLATFORM}" = "ccimx8mn-dvk" ] || [ "${PLATFORM}" = "ccimx8mm-dvk" ]; then
			SKIP_BLOCKS="0"
	                DEK_BLOB_HEADER="81 00 48 4"
		else
			SKIP_BLOCKS="2"
			DEK_BLOB_HEADER="81 00 58 4"
		fi

		dd if=${UBOOT_MMC_DEV} of=${UBOOT_MMC_DUMP} count=1 skip=${SKIP_BLOCKS} 2>/dev/null
		uboot_start="0x$(hexdump -C ${UBOOT_MMC_DUMP} | grep -m 1 "${UBOOT_HEADER}" | head -1 | cut -c -8)"
		echo "++++ ${uboot_start} +++"
		if [ "${uboot_start}" = "0x" ]; then
			echo "Could not find U-Boot on MMC"
			return 1
		fi

		uboot_size_offset="$((uboot_start + 36))"
		uboot_size=$(hexdump -n 4 -s ${uboot_size_offset} -e '/4 "%d\t" "\n"' ${UBOOT_MMC_DUMP})

		# DEK blobs have an overhead of 56 bytes.
		dek_blob_size="$((KEY_SIZE_BYTES + 56))"

		# remove the output DEK file before creating it.
		# Since this function is called twice.
		# For the actual upgrade and then for the validation after the upgrade.
		rm -f ${OUTPUT_FILE}
		dump_size="$((uboot_size / 512))"
		echo "++++ ${dump_size} +++"
		dd if=${UBOOT_MMC_DEV} of=${UBOOT_MMC_DUMP} count=${dump_size} skip=${SKIP_BLOCKS} conv=fsync 2>/dev/null
		dek_start=$(hexdump -C ${UBOOT_MMC_DUMP} | grep -m 1 "${DEK_BLOB_HEADER}" | head -1 | cut -c -8)
		echo "++++ dek_start ${dek_start} +++"
		dek_start="$((16#${dek_start} + 8))"
		echo "++++ dek_start ${dek_start} +++"
		dd of=${OUTPUT_FILE} if=${UBOOT_MMC_DUMP} count=${dek_blob_size} bs=1 skip=${dek_start} 2>/dev/null
		rc=$?
		if [ $rc -ne 0 ]; then
			echo "DEK dump to the output file failed."
			return $rc
		fi
		echo "dump_dek: output file has been created."
		# Validate DEK blob
		if [ -z "$(dd if=${OUTPUT_FILE} bs=1 count=4 2>/dev/null | hexdump -C | grep "${DEK_BLOB_HEADER}")" ]; then
			echo "Could not find DEK blob"
			rm -rf ${OUTPUT_FILE}
			return 1
		fi
		echo "DEK blob correctly dumped"
		rm -f ${UBOOT_MMC_DUMP}
		return 0
	fi
}

if [ "${UBOOT_ENC}" = "enc" ]; then
	dump_dek
	rc=$?
	if [ "$rc" -ne 0 ]; then
		echo "u-boot: DEK dump failed"
		exit $rc
	fi
	if [ "${PLATFORM}" = "ccimx8x-sbc-pro" ] || [ "${PLATFORM}" = "ccimx8x-sbc-express" ]; then
		cp /tmp/${UBOOT_FILE} /tmp/${ENCRYPTED_UBOOT_DEK}
		# insert the dek_blob into the AHAB container
		dd if=${OUTPUT_FILE} of=/tmp/${ENCRYPTED_UBOOT_DEK} bs=1 seek=${dek_blob} conv=notrunc 2>/dev/null
		rc=$?
		if [ "$rc" -ne 0 ]; then
			echo "u-boot: Merging DEK with U-Boot image failed (DEV/FILE = /tmp/$UBOOT_FILE)"
			exit $rc
		fi
	elif [ "${PLATFORM}" = "ccimx8mn-dvk" ] || [ "${PLATFORM}" = "ccimx8mm-dvk" ]; then
		FIT_DEK_BLOB_SIZE="96";
		cp /tmp/${UBOOT_FILE} /tmp/${ENCRYPTED_UBOOT_DEK}
		# insert the dek_blob into the SPL
		dd if=${OUTPUT_FILE} of=/tmp/${ENCRYPTED_UBOOT_DEK} bs=1 seek=${dek_start} conv=notrunc 2>/dev/null
		rc=$?
		if [ "$rc" -ne 0 ]; then
			echo "u-boot: Merging DEK with SPL image failed (DEV/FILE = /tmp/$UBOOT_FILE)"
			exit $rc
		fi
		# get u-boot image file size
		uboot_file_size="$(stat -L -c %s /tmp/${UBOOT_FILE})"
		echo " ++++ uboot_file_size ${uboot_file_size} ***"
		uboot_dek_blob_offset="$((uboot_file_size - FIT_DEK_BLOB_SIZE))"
		echo " ----- uboot_dek_blob_offset ${uboot_dek_blob_offset} **"
		# insert the dek_blob at the end of the bootloader
		dd of=/tmp/${ENCRYPTED_UBOOT_DEK} if=${OUTPUT_FILE} bs=1 seek=${uboot_dek_blob_offset} conv=notrunc 2>/dev/null
		rc=$?
		if [ "$rc" -ne 0 ]; then
			echo "u-boot: Merging DEK with U-Boot image failed (DEV/FILE = /tmp/$UBOOT_FILE)"
			exit $rc
		fi
	else
		cat /tmp/${UBOOT_FILE} ${OUTPUT_FILE} > /tmp/${ENCRYPTED_UBOOT_DEK}
		rc=$?
		if [ "$rc" -ne 0 ]; then
			echo "u-boot: Merging DEK with U-Boot image failed (DEV/FILE = /tmp/$UBOOT_FILE)"
			exit $rc
		fi
	fi
	# enable write access
	echo 0 > /sys/block/mmcblk0boot0/force_ro
	UBOOT_FILE="/tmp/${ENCRYPTED_UBOOT_DEK}"
	# write the encrypted u-boot into the MMC
	dd if=${UBOOT_FILE} of=${UBOOT_MMC_DEV} seek=${uboot_seek_kb} bs=1K
	rc=$? 2>/dev/null
	if [ "$rc" -ne 0 ]; then
		echo "u-boot: failed to write file ${UBOOT_FILE}"
	else
		echo "u-boot: successfully written file ${UBOOT_FILE}"
	fi
	# disable write access
	echo 1 > /sys/block/mmcblk0boot0/force_ro
	rm -f ${UBOOT_FILE} ${OUTPUT_FILE}
else
	# enable write access
	echo 0 > /sys/block/mmcblk0boot0/force_ro
	# write the u-boot into the MMC
	dd if=${UBOOT_FILE} of=${UBOOT_MMC_DEV} seek=${uboot_seek_kb} bs=1K 2>/dev/null
	rc=$?
	if [ "$rc" -ne 0 ]; then
		echo "u-boot: failed to write file ${UBOOT_FILE}"
	else
		echo "u-boot: successfully written file ${UBOOT_FILE}"
	fi
	# disable write access
	echo 1 > /sys/block/mmcblk0boot0/force_ro
	rm -f ${UBOOT_FILE} ${OUTPUT_FILE}
fi
