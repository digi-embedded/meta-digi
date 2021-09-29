#!/bin/sh
#===============================================================================
#
#  Copyright (C) 2021 by Digi International Inc.
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

# Enable the redirect support to get u-boot variables values
uuu fb: ucmd setenv stdout serial,fastboot

# Check if dualboot variable is active
dualboot=$(getenv "dualboot")
if [ "${dualboot}" = "yes" ]; then
	DUALBOOT=true;
fi

echo ""
echo "Determining image files to use..."

# Determine U-Boot file to program basing on SOM's SOC type (linked to bus width)
if [ -z ${INSTALL_UBOOT_FILENAME} ]; then
	module_variant=$(getenv "module_variant")
	# Determine U-Boot file to program basing on SOM's variant
	if [ -n "$module_variant" ] || [ "$module_variant" = "0x00" ]; then
		if [ "$module_variant" = "0x01" ] || \
		   [ "$module_variant" = "0x02" ]; then
			module_ram="2GB"
		else
			module_ram="1GB"
		fi
		INSTALL_UBOOT_FILENAME="u-boot-##MACHINE##${module_ram}.imx"
	fi

	# U-Boot when the checked value is empty.
	if [ -n "${INSTALL_UBOOT_FILENAME}" ]; then
		true
	else
		# remove redirect
		uuu fb: ucmd setenv stdout serial

		echo ""
		echo "[ERROR] Cannot determine U-Boot file for this module!"
		echo ""
		echo "1. Set variable 'INSTALL_UBOOT_FILENAME' depending on your ConnectCore 6 QuadPlus variant:"
		echo "   - For a QuadPlus CPU with 2GB DDR3, run:"
		echo "     => setenv INSTALL_UBOOT_FILENAME u-boot-##MACHINE##2GB.imx"
		echo "   - For a DualPlus CPU with 1GB DDR3, run:"
		echo "     => setenv INSTALL_UBOOT_FILENAME u-boot-##MACHINE##1GB.imx"
		echo ""
		echo ""
		echo "2. Run the install script again."
		echo ""
		echo "Aborted"
		echo ""
		exit
	fi
fi

# remove redirect
uuu fb: ucmd setenv stdout serial

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

# parts names
LINUX_NAME="linux"
RECOVERY_NAME="recovery"
ROOTFS_NAME="rootfs"
# Print warning about storage media being deleted
if [ "${NOWAIT}" != true ]; then
	WAIT=10
	printf "\n"
	printf " ====================\n"
	printf " =    IMPORTANT!    =\n"
	printf " ====================\n"
	printf " This process will erase your eMMC and will install the following files\n"
	printf " on the partitions of the eMMC.\n"
	printf "\n"
	printf "   PARTITION\tFILENAME\n"
	printf "   ---------\t--------\n"
	printf "   bootloader\t${INSTALL_UBOOT_FILENAME}\n"
	if [ "${DUALBOOT}" = true ]; then
		printf "   ${LINUX_NAME}_a\t${INSTALL_LINUX_FILENAME}\n"
		printf "   ${LINUX_NAME}_b\t${INSTALL_LINUX_FILENAME}\n"
		printf "   ${ROOTFS_NAME}_a\t${INSTALL_ROOTFS_FILENAME}\n"
		printf "   ${ROOTFS_NAME}_b\t${INSTALL_ROOTFS_FILENAME}\n"
	else
		printf "   ${LINUX_NAME}\t${INSTALL_LINUX_FILENAME}\n"
		printf "   ${RECOVERY_NAME}\t${INSTALL_RECOVERY_FILENAME}\n"
		printf "   ${ROOTFS_NAME}\t${INSTALL_ROOTFS_FILENAME}\n"
	fi
	printf "\n"
	printf " Press CTRL+C now if you wish to abort.\n"
	printf "\n"
	while [ ${WAIT} -gt 0 ]; do
		printf "\r Update process starts in %d " ${WAIT}
		sleep 1
		WAIT=$(( ${WAIT} - 1 ))
	done
	printf "\r                                   \n"
	printf " Starting update process\n"
fi

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

# Restore dualboot if previously active
if [ "${DUALBOOT}" = true ]; then
	uuu fb: ucmd setenv dualboot yes
fi

if [ "${DUALBOOT}" = true ]; then
	# Update Linux A
	part_update "${LINUX_NAME}_a" "${INSTALL_LINUX_FILENAME}"
	# Update Linux B
	part_update "${LINUX_NAME}_b" "${INSTALL_LINUX_FILENAME}"
	# Update Rootfs A
	part_update "${ROOTFS_NAME}_a" "${INSTALL_ROOTFS_FILENAME}"
	# Update Rootfs B
	part_update "${ROOTFS_NAME}_b" "${INSTALL_ROOTFS_FILENAME}"
else
	# Update Linux
	part_update "${LINUX_NAME}" "${INSTALL_LINUX_FILENAME}"
	# Update Recovery
	part_update "${RECOVERY_NAME}" "${INSTALL_RECOVERY_FILENAME}"
	# Update Rootfs
	part_update "${ROOTFS_NAME}" "${INSTALL_ROOTFS_FILENAME}"
fi

# If the rootfs image was originally compressed, remove the uncompressed image
if [ -f ${COMPRESSED_ROOTFS_IMAGE} ] && [ -f ${INSTALL_ROOTFS_FILENAME} ]; then
	rm -f "${INSTALL_ROOTFS_FILENAME}"
fi

if [ "${DUALBOOT}" != true ]; then
	# Configure u-boot to boot into recovery mode
	uuu fb: ucmd setenv boot_recovery yes
	uuu fb: ucmd setenv recovery_command wipe_update
fi
uuu fb: ucmd saveenv

# Reset the target
uuu fb: acmd reset

echo "\033[32m"
echo "============================================================="
echo "Done! Wait for the target to complete first boot process."
echo "============================================================="
echo "\033[0m"

exit
