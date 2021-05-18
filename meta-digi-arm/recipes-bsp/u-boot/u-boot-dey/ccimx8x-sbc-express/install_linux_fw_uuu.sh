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
	echo "                          Defaults to 'dey-image-qt' if not provided."
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
if [ -z ${INSTALL_UBOOT_FILENAME} ]; then
	# Enable the redirect support to get u-boot variables values
	uuu fb: ucmd setenv stdout serial,fastboot

	# Since SOMs with the B0 SOC might have an older U-Boot that doesn't export the
	# SOC revision to the environment, use B0 by default
	soc_rev=$(getenv "soc_rev")
	if [ -z "${soc_rev}" ]; then
		soc_rev="B0"
	fi

	bus_width="32bit"
	soc_type=$(getenv "soc_type")
	if [ "$soc_type" = "imx8dx"  ]; then
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
			INSTALL_UBOOT_FILENAME="imx-boot-ccimx8x-sbc-express-${soc_rev}-${module_ram}_${bus_width}.bin"
		fi
	else
		INSTALL_UBOOT_FILENAME="imx-boot-ccimx8x-sbc-express-${soc_rev}-${module_ram}_${bus_width}.bin"
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
		echo "1. Add U-boot file name, depending on your ConnectCore 8X variant, to script command line:"
		echo "   - For a QuadXPlus CPU with 1GB LPDDR4, run:"
		echo "     => ./install_linux_fs_uuu.sh -u imx-boot-ccimx8x-sbc-express-${soc_rev}-1GB_32bit.bin"
		echo "   - For a QuadXPlus CPU with 2GB LPDDR4, run:"
		echo "     => ./install_linux_fs_uuu.sh -u imx-boot-ccimx8x-sbc-express-${soc_rev}-2GB_32bit.bin"
		echo "   - For a DualX CPU with 1GB LPDDR4, run:"
		echo "     => ./install_linux_fs_uuu.sh -u imx-boot-ccimx8x-sbc-express-${soc_rev}-1GB_16bit.bin"
		echo "   - For a DualX CPU with 512MB LPDDR4, run:"
		echo "     => ./install_linux_fs_uuu.sh -u imx-boot-ccimx8x-sbc-express-${soc_rev}-512MB_16bit.bin"
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
	IMAGE_NAME="dey-image-qt"
fi
INSTALL_LINUX_FILENAME="${IMAGE_NAME}-##GRAPHICAL_BACKEND##-ccimx8x-sbc-express.boot.vfat"
INSTALL_RECOVERY_FILENAME="${IMAGE_NAME}-##GRAPHICAL_BACKEND##-ccimx8x-sbc-express.recovery.vfat"
INSTALL_ROOTFS_FILENAME="${IMAGE_NAME}-##GRAPHICAL_BACKEND##-ccimx8x-sbc-express.ext4"

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
	fastboot 1;
"

uuu fb: ucmd saveenv
uuu fb: acmd reset

# Wait for the target to reset
sleep 3

# Set fastboot buffer address to $loadaddr, just in case
uuu fb: ucmd setenv fastboot_buffer \${loadaddr}

# Update Linux
part_update "linux" "${INSTALL_LINUX_FILENAME}"

# Update Recovery
part_update "recovery" "${INSTALL_RECOVERY_FILENAME}"

# Update Rootfs
part_update "rootfs" "${INSTALL_ROOTFS_FILENAME}"

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
