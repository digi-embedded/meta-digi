#!/bin/sh
#===============================================================================
#
#  Copyright (C) 2024 by Digi International Inc.
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
	echo "   -a <atf-filename>      Arm-trusted-firmware filename."
	echo "                          Auto-determined by variant if not provided."
	echo "   -b                     Activate bootcount mechanism (3 boot attempts)."
	echo "   -d                     Install firmware on dualboot partitions (system A and system B)."
	echo "                          (Implies -b)."
	echo "   -f <fip-filename>      FIP filename."
	echo "                          Auto-determined by variant if not provided."
	echo "   -h                     Show this help."
	echo "   -i <dey-image-name>    Image name that prefixes the image filenames, such as 'dey-image-qt', "
	echo "                          'dey-image-webkit', 'core-image-base'..."
	echo "                          Defaults to '##DEFAULT_IMAGE_NAME##' if not provided."
	echo "   -n                     No wait. Skips 10 seconds delay to stop script."
	echo "   -t                     Install TrustFence artifacts."
	exit 2
}

# Update a partition
#   Params:
#	1. partition
#	2. file
#	3. timeout (in ms)
part_update()
{
	printf "\033[36m\n"
	printf "=====================================================================================\n"
	printf "Updating '%s' partition with file: %s\n" "${1}" "${2}"
	printf "=====================================================================================\n"
	printf "\033[0m\n"

	if echo "${1}" | grep -qs "^boot[0-9]$"; then
		uuu "fb[-t ${3}]:" flash "${1}" "${2}"
	elif echo "${2}" | grep -qs "\.gz$"; then
		uuu "fb[-t ${3}]:" flash -raw2sparse "${1}" "${2}/*"
	else
		uuu "fb[-t ${3}]:" flash -raw2sparse "${1}" "${2}"
	fi
}

clear
echo "############################################################"
echo "#           Linux firmware install through USB OTG         #"
echo "############################################################"

# Command line admits the following parameters:
# -a <atf-filename>
# -b, -d, -n (booleans)
# -f <fip-filename>
# -i <image-name>
while getopts 'a:bdf:hi:nt' c
do
	case $c in
	a) INSTALL_ATF_FILENAME=${OPTARG} ;;
	b) BOOTCOUNT=true ;;
	d) INSTALL_DUALBOOT=true && BOOTCOUNT=true ;;
	f) INSTALL_FIP_FILENAME=${OPTARG} ;;
	h) show_usage ;;
	i) IMAGE_NAME=${OPTARG} ;;
	n) NOWAIT=true ;;
	t) TRUSTFENCE=true ;;
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

# Determine ATF file to program
if [ -z "${INSTALL_ATF_FILENAME}" ]; then
	INSTALL_ATF_FILENAME="tf-a-##MACHINE##-emmc.stm32##SIGNED_TFA##"
fi
INSTALL_METADATA_FILENAME="metadata-##MACHINE##.bin"

# Determine FIP file to program
if [ -z "${INSTALL_FIP_FILENAME}" ]; then
	INSTALL_FIP_FILENAME="fip-##MACHINE##-optee##SIGNED##.bin"
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

# Verify existence of files before starting the update
FILES="${INSTALL_ATF_FILENAME} ${INSTALL_METADATA_FILENAME} ${INSTALL_FIP_FILENAME} ${INSTALL_LINUX_FILENAME}"
if [ "${DUALBOOT}" != true ]; then
	FILES="${FILES} ${INSTALL_RECOVERY_FILENAME}"
fi
for f in ${FILES}; do
	if [ ! -f "${f}" ]; then
		printf "\033[31m[ERROR] Could not find file '%s'\033[0m\n" "${f}"
		ABORT=true
	fi
done;

# Verify what kind of rootfs is going to be programmed
ROOTFS_FILENAME="${BASEFILENAME}-##MACHINE##.ext4"
ROOTFS_FILENAME_GZ="${ROOTFS_FILENAME}.gz"
ROOTFS_FILENAME_SQFS="${BASEFILENAME}-##MACHINE##.squashfs"
if [ -f "${ROOTFS_FILENAME_GZ}" ]; then
	INSTALL_ROOTFS_FILENAME="${ROOTFS_FILENAME_GZ}"
elif [ -f "${ROOTFS_FILENAME}" ]; then
	INSTALL_ROOTFS_FILENAME="${ROOTFS_FILENAME}"
elif [ -f "${ROOTFS_FILENAME_SQFS}" ]; then
	INSTALL_ROOTFS_FILENAME="${ROOTFS_FILENAME_SQFS}"
else
	printf "\033[31m[ERROR] Could not find any rootfs image\033[0m\n"
	ABORT=true
fi

[ "${ABORT}" = true ] && exit 1

# Enable bootcount mechanism by setting a bootlimit
if [ "${BOOTCOUNT}" = true ]; then
	bootlimit_cmd="setenv bootlimit 3"
fi

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
	printf "   boot1\t%s\n" "${INSTALL_ATF_FILENAME}"
	printf "   boot2\t%s\n" "${INSTALL_ATF_FILENAME}"
	printf "   metadata1\t%s\n" "${INSTALL_METADATA_FILENAME}"
	printf "   metadata2\t%s\n" "${INSTALL_METADATA_FILENAME}"
	printf "   fip-a\t%s\n" "${INSTALL_FIP_FILENAME}"
	printf "   fip-b\t%s\n" "${INSTALL_FIP_FILENAME}"
	if [ "${DUALBOOT}" = true ]; then
		printf "   %s_a\t%s\n" "${LINUX_NAME}" "${INSTALL_LINUX_FILENAME}"
		if [ "${INSTALL_DUALBOOT}" = true ]; then
			printf "   %s_b\t%s\n" "${LINUX_NAME}" "${INSTALL_LINUX_FILENAME}"
		fi
		printf "   %s_a\t%s\n" "${ROOTFS_NAME}" "${INSTALL_ROOTFS_FILENAME}"
		if [ "${INSTALL_DUALBOOT}" = true ]; then
			printf "   %s_b\t%s\n" "${ROOTFS_NAME}" "${INSTALL_ROOTFS_FILENAME}"
		fi
	else
		printf "   %s\t%s\n" "${LINUX_NAME}" "${INSTALL_LINUX_FILENAME}"
		printf "   %s\t%s\n" "${RECOVERY_NAME}" "${INSTALL_RECOVERY_FILENAME}"
		printf "   %s\t%s\n" "${ROOTFS_NAME}" "${INSTALL_ROOTFS_FILENAME}"
	fi
	printf "\n"
	printf " Press CTRL+C now if you wish to abort.\n"
	printf "\n"
	while [ ${WAIT} -gt 0 ]; do
		printf "\r Update process starts in %d " ${WAIT}
		sleep 1
		WAIT="$((WAIT - 1))"
	done
	printf "\r                                   \n"
	printf " Starting update process\n"
fi

# Skip user confirmation for U-Boot update
uuu fb: ucmd setenv forced_update 1

# Update ATF
part_update "boot1" "${INSTALL_ATF_FILENAME}" 5000
part_update "boot2" "${INSTALL_ATF_FILENAME}" 5000

# Update metadata
part_update "metadata1" "${INSTALL_METADATA_FILENAME}" 5000
part_update "metadata2" "${INSTALL_METADATA_FILENAME}" 5000

# Update FIP
part_update "fip-a" "${INSTALL_FIP_FILENAME}" 5000
part_update "fip-b" "${INSTALL_FIP_FILENAME}" 5000

# Set 'bootcmd' for the second part of the script that will
#  - Reset environment to defaults
#  - Keep the 'dualboot' status
#  - Reset the bootcount
#  - Set bootlimit (if required)
#  - Save the environment
#  - Update the 'linux' partition(s)
#  - Update the 'rootfs' partition(s)
uuu fb: ucmd setenv bootcmd "
	env default -a;
	setenv dualboot \${dualboot};
	bootcount reset;
	${bootlimit_cmd};
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

if [ "${DUALBOOT}" = true ]; then
	# Update Linux A
	part_update "${LINUX_NAME}_a" "${INSTALL_LINUX_FILENAME}" 15000
	# Update Linux B
	if [ "${INSTALL_DUALBOOT}" = true ]; then
		part_update "${LINUX_NAME}_b" "${INSTALL_LINUX_FILENAME}" 15000
	fi
	# Update Rootfs A
	part_update "${ROOTFS_NAME}_a" "${INSTALL_ROOTFS_FILENAME}" 120000
	# Update Rootfs B
	if [ "${INSTALL_DUALBOOT}" = true ]; then
		part_update "${ROOTFS_NAME}_b" "${INSTALL_ROOTFS_FILENAME}" 120000
	fi
else
	# Update Linux
	part_update "${LINUX_NAME}" "${INSTALL_LINUX_FILENAME}" 15000
	# Update Recovery
	part_update "${RECOVERY_NAME}" "${INSTALL_RECOVERY_FILENAME}" 15000
	# Update Rootfs
	part_update "${ROOTFS_NAME}" "${INSTALL_ROOTFS_FILENAME}" 120000
	# Configure u-boot to boot into recovery mode and format the
	# 'update' partition
	uuu fb: ucmd setenv boot_recovery yes
	uuu fb: ucmd setenv recovery_command wipe_update
	uuu fb: ucmd saveenv
fi

# Set the dboot_kernel_var to fitimage if Trustfence is enabled
if [ "${TRUSTFENCE}" = "true" ] || echo "${INSTALL_UBOOT_FILENAME}" | grep -q -e "signed"; then
	uuu fb: ucmd setenv dboot_kernel_var fitimage
	uuu fb: ucmd saveenv
fi

# Reset the bootcount
uuu fb: acmd bootcount reset
# Reset the target
uuu fb: acmd reset

printf "\033[32m\n"
printf "=============================================================\n"
printf "Done! Wait for the target to complete first boot process.\n"
printf "=============================================================\n"
printf "\033[0m\n"

exit
