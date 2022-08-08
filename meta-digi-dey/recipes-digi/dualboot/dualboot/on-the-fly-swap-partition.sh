#!/bin/sh
#===============================================================================
#
#  on-the-fly-swap-partition.sh
#
#  Copyright (C) 2021-2022 by Digi International Inc.
#  All rights reserved.
#
#  This program is free software; you can redistribute it and/or modify it
#  under the terms of the GNU General Public License version 2 as published by
#  the Free Software Foundation.
#
#
#  !Description: On the fly script to swap active partition
#
#===============================================================================

ACTIVE_SYSTEM="$(fw_printenv -n active_system 2>/dev/null)"

# Get current partition information so we can
# determine where to flash the images.
if [ "${ACTIVE_SYSTEM}" = "linux_a" ]; then
	KERNELBOOT="linux_b"
	ROOTFS="rootfs_b"
else
	KERNELBOOT="linux_a"
	ROOTFS="rootfs_a"
fi
if grep -qs environment /proc/mtd; then
	fw_setenv mtdbootpart ${KERNELBOOT}
	fw_setenv mtdrootfspart ${ROOTFS}
	fw_setenv rootfsvol ${ROOTFS}
	fw_setenv active_system ${KERNELBOOT}
else
	# Get Boot partition device and index.
	BOOT_DEV="$(fw_printenv -n mmcbootdev 2>/dev/null)"

	# get boot partition index
	MMC_PART="$(ls -l /dev/disk/by-partlabel/ | grep -i ${KERNELBOOT} | awk '{print $11}' | sed -e 's/[../mmcblkp]//g' -e 's/^.//')"

	# search rootfs UUID
	MMCROOT_PART="$(ls -l /dev/disk/by-partlabel/ | grep -i ${ROOTFS} | awk '{print $11}' | sed -e 's/[../mmcblkp]//g' -e 's/^.//')"
	PART_UUID="$(ls -l /dev/disk/by-partuuid/ | grep -i mmcblk${BOOT_DEV}p${MMCROOT_PART} | awk '{print $9}')"

	fw_setenv mmcroot PARTUUID=${PART_UUID}
	fw_setenv mmcpart ${MMC_PART}
	fw_setenv active_system ${KERNELBOOT}
fi
