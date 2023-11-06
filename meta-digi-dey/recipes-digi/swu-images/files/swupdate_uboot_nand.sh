#!/bin/sh
#===============================================================================
#
#  Copyright (C) 2022-2023 by Digi International Inc.
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
TFA_FILE="$4"

echo "**** Start U-Boot update process *****"

# need to mount debufs to remove some kobs-ng warnings
if ! grep -qs debugfs /proc/mounts; then
	mount -t debugfs debugfs /sys/kernel/debug/
fi

dump_dek ()
{
	echo "**** Get DEK and append to the new u-boot *****"
	UBOOT_MTD_DEV="/dev/mtd0"
	OUTPUT_FILE="/tmp/dek.bin"
	KEY_SIZE_BYTES="32"
	ENCRYPTED_UBOOT_DEK="u-boot-encrypted-with-dek.imx"

	#(The last byte lacks one digit on purpose, to match 40, 41 and 42; all HAB versions)
	UBOOT_HEADER="d1 00 20 4"
	DEK_BLOB_HEADER="81 00 58 4"

	uboot_start="0x$(nanddump ${UBOOT_MTD_DEV} | hexdump -C | grep -m 1 "${UBOOT_HEADER}" | head -1 | cut -c -8)"
	if [ "${uboot_start}" = "0x" ]; then
		echo "Could not find U-Boot on NAND"
		return 78
	fi

	uboot_size_offset=$((uboot_start + 36))
	uboot_size=$(hexdump -n 4 -s ${uboot_size_offset} -e '/4 "0x%08x\t" "\n"' ${UBOOT_MTD_DEV})
	# dump start needs to be aligned (U-Boot always leaves 0x400 for DOS table)
	dump_start=$((uboot_start - 0x400))
	# DEK blobs have an overhead of 56 bytes.
	dek_blob_size=$((KEY_SIZE_BYTES + 56))

	# remove the output DEK file before creating it.
	# Since this function is called twice.
	# For the actual upgrade and then for the validation after the upgrade.
	rm -f ${OUTPUT_FILE}
	# read the complete U-Boot (to skip alignment issues) and keep the dek_blob (which is at the end)
	nanddump -s ${dump_start} -l ${uboot_size} ${UBOOT_MTD_DEV} | tail -c ${dek_blob_size} > ${OUTPUT_FILE}
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
		return 60
	fi
	echo "DEK blob correctly dumped"
	return 0
}

install_fwu ()
{
	FLASH_DEV="$1"
	FW_FILE="$2"

	flash_eraseall ${FLASH_DEV}
	rc=$?
	if [ "$rc" -ne 0 ]; then
		echo "U-Boot: erasing ${FLASH_DEV} failed"
		exit $rc
	fi
	nandwrite -p ${FLASH_DEV} /tmp/${FW_FILE}
	if [ "$rc" -ne 0 ]; then
		echo "U-Boot: failed to write firmware to ${FLASH_DEV}"
		exit $rc
	fi
}

COMP_CCMP1=$(cat /proc/device-tree/compatible | grep "\bdigi,ccmp1\b")

if [ "${UBOOT_ENC}" = "enc" ]; then
	if [ "${COMP_CCMP1}" = "digi,ccmp1" ]; then
		# Currently not supported for these platforms
		echo "*** Encrypted U-boot currently not support for CCMP1 ***" 
	else
		dump_dek
		rc=$?
		if [ "$rc" -ne 0 ]; then
			echo "u-boot: DEK dump failed"
			exit $rc
		fi
		cat $UBOOT_FILE $OUTPUT_FILE > /tmp/$ENCRYPTED_UBOOT_DEK
		rc=$?
		if [ "$rc" -ne 0 ]; then
			echo "u-boot: Merging DEK with U-Boot image failed (DEV/FILE = $UBOOT_FILE)"
			exit $rc
		fi
		UBOOT_FILE="${ENCRYPTED_UBOOT_DEK}"
	fi
fi

if [ "${COMP_CCMP1}" = "digi,ccmp1" ]; then
	# install TF-A onto fsbl1 partition
	mtd_num="$(sed -ne "/fsbl1/s,^mtd\([0-9]\+\).*,\1,g;T;p" /proc/mtd)"
	TFA_DEV="/dev/mtd${mtd_num}"
	install_fwu ${TFA_DEV} ${TFA_FILE}
	# install U-Boot onto FIP-a partition
	mtd_num="$(sed -ne "/fip-a/s,^mtd\([0-9]\+\).*,\1,g;T;p" /proc/mtd)"
	FIP_DEV="/dev/mtd${mtd_num}"
	install_fwu ${FIP_DEV} ${UBOOT_FILE}
else
	# install U-Boot onto the Nand Flash
	kobs-ng init -x -v /tmp/${UBOOT_FILE}
fi
rc=$?
if [ "$rc" -ne 0 ]; then
	echo "u-Boot: Updating U-Boot partition failed"
else
	echo "u-Boot: Updating U-Boot partition successful"
fi
