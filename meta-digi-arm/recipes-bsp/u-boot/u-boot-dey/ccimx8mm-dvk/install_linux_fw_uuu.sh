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
# U-Boot script for installing Linux images created by Yocto into the eMMC
#
clear

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
	echo "                          Defaults to 'dey-image-qt' if not provided."
	echo "   -n                     No wait. Skips 10 seconds delay to stop script."
	echo "   -u <u-boot-filename>   U-Boot filename."
	echo "                          Auto-determined by variant if not provided."
	exit 2
}

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

if [ ! "${NOWAIT}" = true ]; then
	echo "############################################################"
	echo "#           Linux firmware install through USB OTG         #"
	echo "############################################################"
	echo ""
	echo " This process will erase your eMMC and will install a new"
	echo " U-Boot and Linux firmware images on the eMMC."
	echo ""
	echo " Press CTRL+C now if you wish to abort or wait 10 seconds"
	echo " to continue."
	sleep 10
fi

if [ -z "${INSTALL_UBOOT_FILENAME}" ]; then
	INSTALL_UBOOT_FILENAME="imx-boot-ccimx8mm-dvk.bin"
fi

# Skip user confirmation for U-Boot update
uuu fb: ucmd setenv forced_update 1

# Update U-Boot
uuu fb: flash bootloader ${INSTALL_UBOOT_FILENAME}

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

if [ -z "${IMAGE_NAME}" ]; then
	IMAGE_NAME="dey-image-qt"
fi
INSTALL_LINUX_FILENAME="${IMAGE_NAME}-##GRAPHICAL_BACKEND##-ccimx8mm-dvk.boot.vfat"
INSTALL_RECOVERY_FILENAME="${IMAGE_NAME}-##GRAPHICAL_BACKEND##-ccimx8mm-dvk.recovery.vfat"
INSTALL_ROOTFS_FILENAME="${IMAGE_NAME}-##GRAPHICAL_BACKEND##-ccimx8mm-dvk.ext4"

# Wait that target returns from reset
sleep 3

# Update Linux
uuu fb: flash -raw2sparse linux ${INSTALL_LINUX_FILENAME}

# Update Recovery
uuu fb: flash -raw2sparse recovery ${INSTALL_RECOVERY_FILENAME}

# Update Rootfs
uuu fb: flash -raw2sparse rootfs ${INSTALL_ROOTFS_FILENAME}

# Configure u-boot to boot into recovery mode
uuu fb: ucmd setenv boot_recovery yes
uuu fb: ucmd setenv recovery_command wipe_update

uuu fb: ucmd saveenv

# Reset the target
uuu fb: acmd reset

exit
