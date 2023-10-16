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

check_cmd()
{
	uuu -v fb: acmd ${1} > /dev/null 2> /dev/null
	uuu -v fb: ucmd echo retval=\$? | sed  -ne "s,^retval=,,g;T;p"
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
	exit 2
}

# Update a partition
#   Params:
#	1. partition
#	2. file
#	3. timeout (in ms)
#   Description:
#	- downloads image to RAM
#	- runs 'update' command from RAM
part_update()
{
	echo "\033[36m"
	echo "====================================================================================="
	echo "Updating '${1}' partition with file: ${2}"
	echo "====================================================================================="
	echo "\033[0m"

	uuu fb: download -f "${2}"
	uuu "fb[-t ${3}]:" ucmd update "${1}" ram \${fastboot_buffer} \${filesize} ${ERASE}
}

clear
echo "############################################################"
echo "#           Linux firmware install through USB OTG         #"
echo "############################################################"

# Command line admits the following parameters:
# -a <atf-filename>
# -f <fip-filename>
# -i <image-name>
while getopts 'a:bdf:hi:n' c
do
	case $c in
	a) INSTALL_ATF_FILENAME=${OPTARG} ;;
	b) BOOTCOUNT=true ;;
	d) INSTALL_DUALBOOT=true && BOOTCOUNT=true ;;
	f) INSTALL_FIP_FILENAME=${OPTARG} ;;
	h) show_usage ;;
	i) IMAGE_NAME=${OPTARG} ;;
	n) NOWAIT=true ;;
	esac
done

# Enable the redirect support to get u-boot variables values
uuu fb: ucmd setenv stdout serial,fastboot

# Check if dualboot variable is active
dualboot=$(getenv "dualboot")
if [ "${dualboot}" = "yes" ]; then
	DUALBOOT=true;
fi

# Check if uboot_config volume exists (U-Boot env)
uuu "fb[-t 15000]:" ucmd ubi part UBI
check=$(check_cmd "ubi check uboot_config")
if [ "${check}" = "1" ]; then
	RUNVOLS=true
fi

# remove redirect
uuu fb: ucmd setenv stdout serial

echo ""
echo "Determining image files to use..."

# Determine ATF file to program
if [ -z "${INSTALL_ATF_FILENAME}" ]; then
	INSTALL_ATF_FILENAME="tf-a-##MACHINE##-nand.stm32"
fi

# Determine FIP file to program
if [ -z "${INSTALL_FIP_FILENAME}" ]; then
	INSTALL_FIP_FILENAME="fip-##MACHINE##-optee.bin"
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
INSTALL_LINUX_FILENAME="${BASEFILENAME}-##MACHINE##.boot.ubifs"
INSTALL_RECOVERY_FILENAME="${BASEFILENAME}-##MACHINE##.recovery.ubifs"
INSTALL_ROOTFS_FILENAME="${BASEFILENAME}-##MACHINE##.ubifs"

# Verify existence of files before starting the update
FILES="${INSTALL_ATF_FILENAME} ${INSTALL_FIP_FILENAME} ${INSTALL_LINUX_FILENAME} ${INSTALL_RECOVERY_FILENAME}"
for f in ${FILES}; do
	if [ ! -f ${f} ]; then
		echo "\033[31m[ERROR] Could not find file '${f}'\033[0m"
		ABORT=true
	fi
done;

# Verify what kind of rootfs is going to be programmed
if [ ! -f ${INSTALL_ROOTFS_FILENAME} ]; then
	echo "\033[31m[ERROR] Could not find file '${INSTALL_ROOTFS_FILENAME}'\033[0m"
	INSTALL_ROOTFS_FILENAME="${BASEFILENAME}-##MACHINE##.squashfs"
	echo "\033[32m[INFO] Trying with file '${INSTALL_ROOTFS_FILENAME}'\033[0m"
	if [ -f "${INSTALL_ROOTFS_FILENAME}" ]; then
		SQUASHFS=true
	else
		echo "\033[31m[ERROR] Could not find file '${INSTALL_ROOTFS_FILENAME}'\033[0m"
		ABORT=true
	fi
fi

# Enable bootcount mechanism by setting a bootlimit
if [ "${BOOTCOUNT}" = true ]; then
	bootlimit_cmd="setenv bootlimit 3"
fi

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
	printf " This process will erase your NAND and will install the following files\n"
	printf " on the partitions of the NAND.\n"
	printf "\n"
	printf "   PARTITION\tFILENAME\n"
	printf "   ---------\t--------\n"
	printf "   fsbl1\t${INSTALL_ATF_FILENAME}\n"
	printf "   fsbl2\t${INSTALL_ATF_FILENAME}\n"
	printf "   fip-a\t${INSTALL_FIP_FILENAME}\n"
	printf "   fip-b\t${INSTALL_FIP_FILENAME}\n"
	if [ "${DUALBOOT}" = true ]; then
		printf "   ${LINUX_NAME}_a\t${INSTALL_LINUX_FILENAME}\n"
		if [ "${INSTALL_DUALBOOT}" = true ]; then
			printf "   ${LINUX_NAME}_b\t${INSTALL_LINUX_FILENAME}\n"
		fi
		printf "   ${ROOTFS_NAME}_a\t${INSTALL_ROOTFS_FILENAME}\n"
		if [ "${INSTALL_DUALBOOT}" = true ]; then
			printf "   ${ROOTFS_NAME}_b\t${INSTALL_ROOTFS_FILENAME}\n"
		fi
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

# Set fastboot buffer address to $loadaddr, just in case
uuu fb: ucmd setenv fastboot_buffer \${loadaddr}

# Skip user confirmation for U-Boot update
uuu fb: ucmd setenv forced_update 1

# Update ATF
part_update "fsbl1" "${INSTALL_ATF_FILENAME}" 5000
part_update "fsbl2" "${INSTALL_ATF_FILENAME}" 5000

# Update FIP
part_update "fip-a" "${INSTALL_FIP_FILENAME}" 5000
part_update "fip-b" "${INSTALL_FIP_FILENAME}" 5000

# Environment volume does not exist and needs to be created
if [ "${RUNVOLS}" = true ]; then
	# Create UBI volumes
	uuu "fb[-t 45000]:" ucmd run ubivolscript
fi

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
	saveenv;
	echo \"\";
	echo \"\";
	echo \">> Installing Linux firmware\";
	echo \"\";
	echo \"\";
	fastboot 0;
"

uuu fb: ucmd saveenv
uuu fb: acmd reset

# Wait for the target to reset
sleep 8

# Set fastboot buffer address to $loadaddr
uuu fb: ucmd setenv fastboot_buffer \${loadaddr}

# Create UBI volumes
uuu "fb[-t 45000]:" ucmd run ubivolscript

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

# Set the rootfstype if squashfs
if [ "${SQUASHFS}" = true ]; then
	uuu fb: ucmd setenv rootfstype squashfs
	uuu fb: ucmd saveenv
fi

# Reset the bootcount
uuu fb: ucmd bootcount reset
# Reset the target
uuu fb: acmd reset

echo "\033[32m"
echo "============================================================="
echo "Done! Wait for the target to complete first boot process."
echo "============================================================="
echo "\033[0m"

exit
