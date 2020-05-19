#!/bin/bash
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

# Enable the redirect support to get u-boot variables values
uuu fb: ucmd setenv stdout serial,fastboot

# Determine U-Boot file to program basing on SOM's SOC type (linked to bus width)
bus_width="32bit"

soc_type=$(getenv "soc_type")
if [ "$soc_type" == "imx8dx"  ]; then
	bus_width="16bit"
fi

module_ram=$(getenv "module_ram")
if [ -z "${module_ram}" ]; then
	module_variant=$(getenv "module_variant")
	# Determine U-Boot file to program basing on SOM's variant
	if [ -n "$module_variant" ]; then
		if [ "$module_variant" == "0x01" ] || \
		     [ "$module_variant" == "0x04" ] || \
		     [ "$module_variant" == "0x05" ]; then
			module_ram="1GB"
		elif [ "$module_variant" == "0x02" ] || \
		     [ "$module_variant" == "0x03" ]; then
			module_ram="2GB"
		else
			module_ram="512MB"
		fi
		INSTALL_UBOOT_FILENAME="imx-boot-ccimx8x-sbc-pro-1.2GHz_${module_ram}_${bus_width}.bin"
	fi
else
	INSTALL_UBOOT_FILENAME="imx-boot-ccimx8x-sbc-pro-1.2GHz_${module_ram}_${bus_width}.bin"
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
	echo "   - For a QuadXPlus CPU with 1GB LPDDR4, run:"
	echo "     => sudo ./install_linux_fs_uuu.sh imx-boot-ccimx8x-sbc-pro-1.2GHz_1GB_32bit.bin"
	echo "   - For a QuadXPlus CPU with 2GB LPDDR4, run:"
	echo "     => sudo ./install_linux_fs_uuu.sh imx-boot-ccimx8x-sbc-pro-1.2GHz_2GB_32bit.bin"
	echo "   - For a DualX CPU with 1GB LPDDR4, run:"
	echo "     => sudo ./install_linux_fs_uuu.sh imx-boot-ccimx8x-sbc-pro-1.2GHz_1GB_16bit.bin"
	echo "   - For a DualX CPU with 512MB LPDDR4, run:"
	echo "     => sudo ./install_linux_fs_uuu.sh imx-boot-ccimx8x-sbc-pro-1.2GHz_512MB_16bit.bin"
	echo ""
	echo "2. Run the install script again."
	echo ""
	echo "Aborted"
	echo ""
	exit
fi

# Skip user confirmation for U-Boot update
uuu fb: ucmd setenv forced_update 1

# Update U-Boot
uuu fb: flash bootloader ${INSTALL_UBOOT_FILENAME}

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
	fastboot 1;
"

uuu fb: ucmd saveenv

uuu fb: acmd reset

INSTALL_LINUX_FILENAME="dey-image-qt-##GRAPHICAL_BACKEND##-ccimx8x-sbc-pro.boot.vfat"
INSTALL_RECOVERY_FILENAME="dey-image-qt-##GRAPHICAL_BACKEND##-ccimx8x-sbc-pro.recovery.vfat"
INSTALL_ROOTFS_FILENAME="dey-image-qt-##GRAPHICAL_BACKEND##-ccimx8x-sbc-pro.ext4"

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
