#!/bin/sh
#===============================================================================
#
#  Copyright (C) 2020 by Digi International Inc.
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
getenv()
{
	uuu -v fb: ucmd printenv "${1}" | sed -ne "s,^${1}=,,g;T;p"
}

# Update a NAND partition
#   Params:
#	1. partition
#	2. file
#	3. timeout (in ms)
#   Description:
#	- downloads image to RAM
#	- runs 'update' command from RAM
nand_update()
{
	echo "\033[36m"
	echo "====================================================================================="
	echo "Updating '${1}' partition with file: ${2}"
	echo "====================================================================================="
	echo "\033[0m"

	uuu fb: download -f "${2}"
	uuu "fb[-t ${3}]:" ucmd update "${1}" ram \${fastboot_buffer} \${fastboot_bytes}
}

#
# U-Boot script for installing Linux images created by Yocto into the NAND
#
echo "############################################################"
echo "#           Linux firmware install through USB OTG         #"
echo "############################################################"
echo ""
echo " This process will erase your NAND and will install a new"
echo " U-Boot and Linux firmware images on the NAND."
echo ""
echo " Press CTRL+C now if you wish to abort or wait 10 seconds"
echo " to continue."

sleep 10

# Enable the redirect support to get u-boot variables values
uuu fb: ucmd setenv stdout serial,fastboot

# Get U-Boot file name from cmdline when passed
if [ -n "$1" ]; then
	INSTALL_UBOOT_FILENAME="$1"
else
	module_variant=$(getenv "module_variant")
	# Determine U-Boot file to program basing on SOM's variant
	if [ -n "$module_variant" ]; then
		if [ "$module_variant" = "0x08" ] || \
		   [ "$module_variant" = "0x09" ]; then
			INSTALL_UBOOT_FILENAME="u-boot-ccimx6ulstarter512MB.imx"
		elif [ "$module_variant" = "0x04" ] || \
		     [ "$module_variant" = "0x05" ] || \
		     [ "$module_variant" = "0x07" ]; then
			INSTALL_UBOOT_FILENAME="u-boot-ccimx6ulstarter1GB.imx"
		else
			INSTALL_UBOOT_FILENAME="u-boot-ccimx6ulstarter.imx"
		fi
	fi
fi

# remove redirect
uuu fb: ucmd setenv stdout serial

# u-boot when the checked value is empty.
if [ -n "${INSTALL_UBOOT_FILENAME}" ]; then
	true
else
	echo ""
	echo "[ERROR] Cannot determine U-Boot file for this module!"
	echo ""
	echo "1. Add U-boot file name, depending on your ConnectCore 8X variant, to script command line:"
	echo "   - For a SOM with 1GB DDR3, run:"
	echo "     => ./install_linux_fs_uuu.sh u-boot-ccimx6ulstarter1GB.imx"
	echo "   - For a SOM with 512MB DDR3, run:"
	echo "     => ./install_linux_fs_uuu.sh u-boot-ccimx6ulstarter512MB.imx"
	echo "   - For a SOM with 256MB DDR3, run:"
	echo "     => ./install_linux_fs_uuu.sh u-boot-ccimx6ulstarter.imx"
	echo ""
	echo "2. Run the install script again."
	echo ""
	echo "Aborted"
	echo ""
	exit
fi

# Set fastboot buffer address to $loadaddr, just in case
uuu fb: ucmd setenv fastboot_buffer \${loadaddr}

# Skip user confirmation for U-Boot update
uuu fb: ucmd setenv forced_update 1

# Update U-Boot
nand_update "uboot" "${INSTALL_UBOOT_FILENAME}" 5000

# Set 'bootcmd' for the second part of the script that will
#  - Reset environment to defaults
#  - Save the environment
#  - Update the 'linux' partition
#  - Update the 'recovery' partition
#  - Update the 'rootfs' partition
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

INSTALL_LINUX_FILENAME="dey-image-qt-##GRAPHICAL_BACKEND##-ccimx6ulstarter.boot.ubifs"
INSTALL_RECOVERY_FILENAME="dey-image-qt-##GRAPHICAL_BACKEND##-ccimx6ulstarter.recovery.ubifs"
INSTALL_ROOTFS_FILENAME="dey-image-qt-##GRAPHICAL_BACKEND##-ccimx6ulstarter.ubifs"

# Wait for the target to reset
sleep 3

# Set fastboot buffer address to $loadaddr
uuu fb: ucmd setenv fastboot_buffer \${loadaddr}

# Update Linux
nand_update "linux" "${INSTALL_LINUX_FILENAME}" 15000

# Update Recovery
nand_update "recovery" "${INSTALL_RECOVERY_FILENAME}" 15000

# Update Rootfs
nand_update "rootfs" "${INSTALL_ROOTFS_FILENAME}" 90000

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
