#!/bin/bash
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

clear

# Parse uuu cmd output
function getenv()
{
	uuu -v fb: ucmd printenv "${1}" | sed -ne "s,^${1}=,,g;T;p"
}

#
# U-Boot script for installing Linux images created by Yocto into the eMMC
#

echo "############################################################"
echo "#           Linux firmware install through USB OTG         #"
echo "############################################################"
echo ""
echo " This process will erase your eMMC and will install a new"
echo " U-Boot and Linux firmware images on the eMMC."
echo ""
echo " Press CTRL+C now if you wish to abort or wait 10 seconds"
echo " to continue."

# Get U-Boot file name from cmdline when passed
if [ -n "$1" ]; then
	INSTALL_UBOOT_FILENAME="$1"
fi

sleep 10

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

INSTALL_LINUX_FILENAME="dey-image-qt-##GRAPHICAL_BACKEND##-ccimx8mm-dvk.boot.vfat"
INSTALL_RECOVERY_FILENAME="dey-image-qt-##GRAPHICAL_BACKEND##-ccimx8mm-dvk.recovery.vfat"
INSTALL_ROOTFS_FILENAME="dey-image-qt-##GRAPHICAL_BACKEND##-ccimx8mm-dvk.ext4"

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
