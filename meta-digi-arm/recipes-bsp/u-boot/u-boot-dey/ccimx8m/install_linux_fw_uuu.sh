#!/bin/sh
#===============================================================================
#
#  Copyright (C) 2020-2021 by Digi International Inc.
#  All rights reserved.
#
#  This program is free software; you can redistribute it and/or modify it
#  under the terms of the GNU General Public License version 2 as published by
#  the Free Software Foundation.
#
#
#  Description:
#    Script to flash Yocto build artifacts over USB to the target.
#===============================================================================
# set -x

#
# U-Boot script for installing Linux images created by Yocto
#

# Exit on any error
set -e

# Parse uuu cmd output
getenv()
{
	uuu -v fb: ucmd printenv "${1}" | sed -ne "s,^${1}=,,g;T;p"
}

show_usage()
{
	echo "Usage: $0 [options]"
	echo ""
	echo "  Options:"
	echo "   -h                     Show this help."
	echo "   -i <dey-image-name>    Image name that prefixes the image filenames, such as 'dey-image-qt', "
	echo "                          'dey-image-webkit', 'core-image-base'..."
	echo "                          Defaults to '##DEFAULT_IMAGE_NAME##' if not provided."
	echo "   -n                     No wait. Skips 10 seconds delay to stop script."
	echo "   -u <u-boot-filename>   U-Boot filename."
	echo "                          Auto-determined by variant if not provided."
	exit 2
}

# Update a partition
#   Params:
#	1. partition
#	2. file
part_update()
{
	echo "\033[36m"
	echo "====================================================================================="
	echo "Updating '${1}' partition with file: ${2}"
	echo "====================================================================================="
	echo "\033[0m"

	if [ "${1}" = "bootloader" ]; then
		uuu fb: flash "${1}" "${2}"
	else
		uuu fb: flash -raw2sparse "${1}" "${2}"
	fi
}

clear
echo "############################################################"
echo "#           Linux firmware install through USB OTG         #"
echo "############################################################"

# Command line admits the following parameters:
# -u <u-boot-filename>
# -i <image-name>
while getopts 'hi:nu:' c
do
	case $c in
	h) show_usage ;;
	i) IMAGE_NAME=${OPTARG} ;;
	n) NOWAIT=true ;;
	u) INSTALL_UBOOT_FILENAME=${OPTARG} ;;
	esac
done

echo ""
echo "Determining image files to use..."

# Determine U-Boot file to program basing on SOM's SOC type (linked to bus width)
if [ -z "${INSTALL_UBOOT_FILENAME}" ]; then
	INSTALL_UBOOT_FILENAME="imx-boot-##MACHINE##.bin"
fi

# Determine linux, recovery, and rootfs image filenames to update
if [ -z "${IMAGE_NAME}" ]; then
	IMAGE_NAME="##DEFAULT_IMAGE_NAME##"
fi
GRAPHICAL_IMAGES="##GRAPHICAL_IMAGES##"
for g in ${GRAPHICAL_IMAGES}; do
	if [ "${IMAGE_NAME}" = "${g}" ]; then
		BASEFILENAME="${IMAGE_NAME}-##GRAPHICAL_BACKEND##"
	fi
done
if [ -z "${BASEFILENAME}" ]; then
	BASEFILENAME="${IMAGE_NAME}"
fi
INSTALL_LINUX_FILENAME="${BASEFILENAME}-##MACHINE##.boot.vfat"
INSTALL_RECOVERY_FILENAME="${BASEFILENAME}-##MACHINE##.recovery.vfat"
INSTALL_ROOTFS_FILENAME="${BASEFILENAME}-##MACHINE##.ext4"

COMPRESSED_ROOTFS_IMAGE="${INSTALL_ROOTFS_FILENAME}.gz"

# If the rootfs image is compressed, make sure to decompress it before the update
if [ -f ${COMPRESSED_ROOTFS_IMAGE} ] && [ ! -f ${INSTALL_ROOTFS_FILENAME} ]; then
	echo "\033[36m"
	echo "====================================================================================="
	echo "Decompressing rootfs image '${COMPRESSED_ROOTFS_IMAGE}'"
	echo "====================================================================================="
	echo "\033[0m"
	gzip -d -k -f "${COMPRESSED_ROOTFS_IMAGE}"
fi

# Verify existance of files before starting the update
FILES="${INSTALL_UBOOT_FILENAME} ${INSTALL_LINUX_FILENAME} ${INSTALL_RECOVERY_FILENAME} ${INSTALL_ROOTFS_FILENAME}"
for f in ${FILES}; do
	if [ ! -f ${f} ]; then
		echo "\033[31m[ERROR] Could not find file '${f}'\033[0m"
		ABORT=true
	fi
done;

[ "${ABORT}" = true ] && exit 1

# Print warning about storage media being deleted
if [ ! "${NOWAIT}" = true ]; then
	WAIT=10
	echo ""
	echo " ===================="
	echo " =    IMPORTANT!    ="
	echo " ===================="
	echo " This process will erase your eMMC and will install the following files"
	echo " on the partitions of the eMMC."
	echo ""
	echo "   PARTITION   FILENAME"
	echo "   ---------   --------"
	echo "   bootloader  ${INSTALL_UBOOT_FILENAME}"
	echo "   linux       ${INSTALL_LINUX_FILENAME}"
	echo "   recovery    ${INSTALL_RECOVERY_FILENAME}"
	echo "   rootfs      ${INSTALL_ROOTFS_FILENAME}"
	echo ""
	echo " Press CTRL+C now if you wish to abort."
	echo ""
	while [ ${WAIT} -gt 0 ]; do
		printf "\r Update process starts in %d " ${WAIT}
		sleep 1
		WAIT=$(( ${WAIT} - 1 ))
	done
	printf "\r                                   \n"
	echo " Starting update process"
fi

# Set fastboot buffer address to $loadaddr, just in case
uuu fb: ucmd setenv fastboot_buffer \${loadaddr}

# Skip user confirmation for U-Boot update
uuu fb: ucmd setenv forced_update 1

# Update U-Boot
part_update "bootloader" "${INSTALL_UBOOT_FILENAME}"

# Set MMC to boot from BOOT1 partition
uuu fb: ucmd mmc partconf 0 1 1 1

# Set 'bootcmd' for the second part of the script that will
#  - Reset environment to defaults
#  - Save the environment
#  - Partition the eMMC user data area for Linux
#  - Update the 'linux' partition
#  - Update the 'recovery' partition
#  - Update the 'rootfs' partition
uuu fb: ucmd setenv bootcmd "
	env default -a;
	saveenv;
	echo \"\";
	echo \"\";
	echo \">> Creating Linux partition table on the eMMC\";
	echo \"\";
	echo \"\";
	run partition_mmc_linux;
	if test \$? -eq 1; then
		echo \"[ERROR] Failed to create Linux partition table!\";
		echo \"\";
		echo \"Aborted.\";
		exit;
	fi;
	echo \"\";
	echo \"\";
	echo \">> Start installation Linux firmware files\";
	echo \"\";
	echo \"\";
	saveenv;
	fastboot 0;
"

uuu fb: ucmd saveenv
uuu fb: acmd reset

# Wait for the target to reset
sleep 3

# Restart fastboot with the latest MMC partition configuration
uuu fb: ucmd setenv fastboot_dev sata
uuu fb: ucmd setenv fastboot_dev mmc

# Set fastboot buffer address to $loadaddr, just in case
uuu fb: ucmd setenv fastboot_buffer \${loadaddr}

# Update Linux
part_update "linux" "${INSTALL_LINUX_FILENAME}"

# Update Recovery
part_update "recovery" "${INSTALL_RECOVERY_FILENAME}"

# Update Rootfs
part_update "rootfs" "${INSTALL_ROOTFS_FILENAME}"

# If the rootfs image was originally compressed, remove the uncompressed image
if [ -f ${COMPRESSED_ROOTFS_IMAGE} ] && [ -f ${INSTALL_ROOTFS_FILENAME} ]; then
	rm -f "${INSTALL_ROOTFS_FILENAME}"
fi

# Configure u-boot to boot into recovery mode
uuu fb: ucmd setenv boot_recovery yes
uuu fb: ucmd setenv recovery_command wipe_update

uuu fb: ucmd saveenv

# Reset the target
uuu fb: acmd reset

echo "\033[32m"
echo "============================================================="
echo "Done! Wait for the target to complete first boot process."
echo "============================================================="
echo "\033[0m"

exit
