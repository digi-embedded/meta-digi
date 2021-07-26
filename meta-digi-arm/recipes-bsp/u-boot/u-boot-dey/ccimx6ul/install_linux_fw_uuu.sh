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
	uuu "fb[-t ${3}]:" ucmd update "${1}" ram \${fastboot_buffer} \${fastboot_bytes}
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

# Enable the redirect support to get u-boot variables values
uuu fb: ucmd setenv stdout serial,fastboot

# Determine U-Boot filename if not provided
if [ -z "${INSTALL_UBOOT_FILENAME}" ]; then
	module_variant=$(getenv "module_variant")
	# Determine U-Boot file to program basing on SOM's variant
	if [ -n "$module_variant" ] || [ "$module_variant" = "0x00" ]; then
		if [ "$module_variant" = "0x08" ] || \
		   [ "$module_variant" = "0x09" ]; then
			INSTALL_UBOOT_FILENAME="u-boot-##MACHINE##512MB.imx"
		elif [ "$module_variant" = "0x04" ] || \
		     [ "$module_variant" = "0x05" ] || \
		     [ "$module_variant" = "0x07" ]; then
			INSTALL_UBOOT_FILENAME="u-boot-##MACHINE##1GB.imx"
		else
			INSTALL_UBOOT_FILENAME="u-boot-##MACHINE##.imx"
		fi
	fi

	# remove redirect
	uuu fb: ucmd setenv stdout serial

	# U-Boot when the checked value is empty.
	if [ -n "${INSTALL_UBOOT_FILENAME}" ]; then
		true
	else
		echo ""
		echo "[ERROR] Cannot determine U-Boot file for this module!"
		echo ""
		echo "1. Add U-boot file name, depending on your ConnectCore 6UL variant, to script command line:"
		echo "   - For a SOM with 1GB DDR3, run:"
		echo "     => ./install_linux_fw_uuu.sh -u u-boot-##MACHINE##1GB.imx"
		echo "   - For a SOM with 512MB DDR3, run:"
		echo "     => ./install_linux_fw_uuu.sh -u u-boot-##MACHINE##512MB.imx"
		echo "   - For a SOM with 256MB DDR3, run:"
		echo "     => ./install_linux_fw_uuu.sh -u u-boot-##MACHINE##.imx"
		echo ""
		echo "2. Run the install script again."
		echo ""
		echo "Aborted"
		echo ""
		exit
	fi
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
	echo " This process will erase your NAND and will install the following files"
	echo " on the partitions of the NAND."
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
part_update "uboot" "${INSTALL_UBOOT_FILENAME}" 5000

# Set 'bootcmd' for the second part of the script that will
#  - Reset environment to defaults
#  - Save the environment
#  - Update the 'linux' partition
#  - Update the 'recovery' partition
#  - Update the 'rootfs' partition
#  - Erase the 'update' partition
uuu fb: ucmd setenv bootcmd "
	env default -a;
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
sleep 3

# Set fastboot buffer address to $loadaddr
uuu fb: ucmd setenv fastboot_buffer \${loadaddr}

# Update Linux
part_update "linux" "${INSTALL_LINUX_FILENAME}" 15000

# Update Recovery
part_update "recovery" "${INSTALL_RECOVERY_FILENAME}" 15000

# Update Rootfs
part_update "rootfs" "${INSTALL_ROOTFS_FILENAME}" 90000

# Erase the 'Update' partition
uuu fb: ucmd nand erase.part update

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
