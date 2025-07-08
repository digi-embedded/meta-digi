#!/bin/sh
#===============================================================================
#
#  Copyright (C) 2020-2025 by Digi International Inc.
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

# Grep for string in command output
# Params:
#  1. Command
#  2. String to grep
grep_string()
{
	uuu -v fb: ucmd ${1} | grep "${2}"
}

show_usage()
{
	echo "Usage: $0 [options]"
	echo ""
	echo "  Options:"
	echo "   -b                     Activate bootcount mechanism (3 boot attempts)."
	echo "   -d                     Install firmware on dualboot partitions (system A and system B)."
	echo "                          (Implies -b)."
	echo "   -h                     Show this help."
	echo "   -i <dey-image-name>    Image name that prefixes the image filenames, such as 'dey-image-qt', "
	echo "                          'dey-image-webkit', 'core-image-base'..."
	echo "                          Defaults to '##DEFAULT_IMAGE_NAME##' if not provided."
	echo "   -k <dek-filename>      Update includes dek file."
	echo "                          (implies -t)."
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
#	4. dek file when updating an encrypted u-boot
part_update()
{
	echo "\033[36m"
	echo "====================================================================================="
	echo "Updating '${1}' partition with file: ${2}"
	echo "====================================================================================="
	echo "\033[0m"

	# When in Multi-MTD mode, pass -e to update command to force the erase
	# of the MTD partition before programming. This is usually done by
	# 'update' command except when a UBI volume is already found.
	# On the install script, the MTD partition table may have changed, so
	# we'd better clean the partition.
	if [ "${SINGLEMTDSYS}" != true ]; then
		ERASE="-e"
	fi
	uuu fb: download -f "${2}"
	if [ "${1}" = "bootloader" ] && [ "${ENCRYPTED}" = "true" ]; then
		if [ -n "${DEK_FILE}" ]; then
			# Encrypted bootloader + dek
			uuu fb: ucmd setenv uboot_size \${filesize}
			uuu fb: ucmd setenv fastboot_buffer \${initrd_addr}
			uuu fb: download -f "${4}"
			uuu fb: ucmd setenv dek_size \${filesize}
			uuu "fb[-t ${3}]:" ucmd trustfence update ram \${loadaddr} \${uboot_size} \${initrd_addr} \${dek_size}
		else
			# Encrypted bootloader (re-use existing dek)
			uuu "fb[-t ${3}]:" ucmd trustfence update ram \${fastboot_buffer} \${fastboot_bytes}
		fi
	else
		# Rest of images (including non-encrypted bootloader)
		uuu "fb[-t ${3}]:" ucmd update "${1}" ram \${fastboot_buffer} \${fastboot_bytes} ${ERASE}
	fi
}

# Format a partition
#   Params:
#	1. partition
#   Description:
#	- erases a partition
#	- creates UBI volume with the same name as the partition
format_part()
{
	echo "\033[36m"
	echo "====================================================================================="
	echo "Formatting '${1}' partition"
	echo "====================================================================================="
	echo "\033[0m"

	uuu "fb[-t 20000]:" ucmd nand erase.part "${1}"
	uuu "fb[-t 20000]:" ucmd if ubi part "${1}"\; then ubi createvol "${1}"\;fi
}

clear
echo "############################################################"
echo "#           Linux firmware install through USB OTG         #"
echo "############################################################"

# Command line admits the following parameters:
# -b, -d, -n (booleans)
# -i <image-name>
# -u <u-boot-filename>
# -k <dek-filename>
while getopts ':bdhi:k:nu:' c
do
	if [ "${c}" = ":" ]; then
		c="${OPTARG}"
		unset OPTARG
	elif echo "${OPTARG}" | grep -qs '^-'; then
		OPTIND="$((OPTIND-1))"
		unset OPTARG
	fi
	case $c in
	b) BOOTCOUNT=true ;;
	d) INSTALL_DUALBOOT=true && BOOTCOUNT=true ;;
	h) show_usage ;;
	i) IMAGE_NAME=${OPTARG} ;;
	k) DEK_FILE=${OPTARG} ;;
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

# Check if singlemtdsys variable is active
singlemtdsys=$(getenv "singlemtdsys")
if [ "${singlemtdsys}" = "yes" ]; then
	SINGLEMTDSYS=true;
fi

echo ""
echo "Determining image files to use..."

# Determine U-Boot filename if not provided
if [ -z "${INSTALL_UBOOT_FILENAME}" ]; then
	module_variant=$(getenv "module_variant")
	# Determine U-Boot file to program basing on SOM's variant
	if [ -n "$module_variant" ]; then
		if [ "$module_variant" = "0x08" ] || \
		   [ "$module_variant" = "0x0a" ]; then
			INSTALL_UBOOT_FILENAME="u-boot-##SIGNED##-##MACHINE##512MB.imx"
		elif [ "$module_variant" = "0x04" ] || \
		     [ "$module_variant" = "0x05" ] || \
		     [ "$module_variant" = "0x07" ]; then
			INSTALL_UBOOT_FILENAME="u-boot-##SIGNED##-##MACHINE##1GB.imx"
		elif [ "$module_variant" = "0x02" ] || \
		     [ "$module_variant" = "0x03" ] || \
		     [ "$module_variant" = "0x06" ] || \
		     [ "$module_variant" = "0x09" ]; then
			INSTALL_UBOOT_FILENAME="u-boot-##SIGNED##-##MACHINE##.imx"
		fi
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
		echo "1. Add U-boot file name, depending on your ConnectCore 6UL variant, to script command line:"
		echo "   - For a SOM with 1GB DDR3, run:"
		echo "     => ./install_linux_fw_uuu.sh -u u-boot-##SIGNED##-##MACHINE##1GB.imx"
		echo "   - For a SOM with 512MB DDR3, run:"
		echo "     => ./install_linux_fw_uuu.sh -u u-boot-##SIGNED##-##MACHINE##512MB.imx"
		echo "   - For a SOM with 256MB DDR3, run:"
		echo "     => ./install_linux_fw_uuu.sh -u u-boot-##SIGNED##-##MACHINE##.imx"
		echo ""
		echo "2. Run the install script again."
		echo ""
		echo "Aborted"
		echo ""
		exit 1
	fi
fi

# Determine if bootloader is signed and/or encrypted
if echo "$INSTALL_UBOOT_FILENAME" | grep -q -e "signed"; then
	SIGNED=true
fi
if echo "$INSTALL_UBOOT_FILENAME" | grep -q -e "encrypted"; then
	ENCRYPTED=true
fi

if [ "${ENCRYPTED}" = "true" ]; then
	tf_status=$(grep_string "trustfence status" "Secure boot:")
	if echo "${tf_status}" | grep -q -e "OPEN"; then
		echo "\033[93m"
		echo "WARNING!"
		echo "You are trying to program encrypted images but the device status is OPEN."
		echo "An OPEN device requires manual procedure for installing an encrypted bootloader,"
		echo "programming the secure keys, and closing the device."
		echo "Continuing would result in a non-secure setup or a non-bootable device after the"
		echo "close operation."
		echo ""
		echo "Check the online documentation for manual steps at:"
		echo "https://docs.digi.com/resources/documentation/digidocs/embedded/trustfence_home.html"
		echo ""
		echo "You can run this installer to program encrypted artifacts when the device has been closed."
		echo "\033[0m"
		echo "Exiting."
		exit 1
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
INSTALL_LINUX_FILENAME="${BASEFILENAME}-##MACHINE##.boot.ubifs"
INSTALL_RECOVERY_FILENAME="${BASEFILENAME}-##MACHINE##.recovery.ubifs"

# Verify existence of files before starting the update
FILES="${INSTALL_UBOOT_FILENAME} ${INSTALL_LINUX_FILENAME}"
if [ "${DUALBOOT}" != true ]; then
	FILES="${FILES} ${INSTALL_RECOVERY_FILENAME}"
fi
for f in ${FILES}; do
	if [ ! -f ${f} ]; then
		echo "\033[31m[ERROR] Could not find file '${f}'\033[0m"
		ABORT=true
	fi
done;

# Verify what kind of rootfs is going to be programmed
ROOTFS_FILENAME="${BASEFILENAME}-##MACHINE##.ubifs"
ROOTFS_FILENAME_SQFS="${BASEFILENAME}-##MACHINE##.squashfs"
if [ -f "${ROOTFS_FILENAME}" ]; then
	INSTALL_ROOTFS_FILENAME="${ROOTFS_FILENAME}"
elif [ -f "${ROOTFS_FILENAME_SQFS}" ]; then
	INSTALL_ROOTFS_FILENAME="${ROOTFS_FILENAME_SQFS}"
	SQUASHFS=true
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
UPDATE_NAME="update"
DATA_NAME="data"

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
	printf "   bootloader\t${INSTALL_UBOOT_FILENAME}\n"
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
	if [ "${SINGLEMTDSYS}" != true ]; then
		if [ "${DUALBOOT}" != true ]; then
			printf "   ${UPDATE_NAME}\t--format--\n"
		fi
		printf "   ${DATA_NAME}\t\t--format--\n"
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

# Update U-Boot
part_update "uboot" "${INSTALL_UBOOT_FILENAME}" 5000 "${DEK_FILE}"

# Set 'bootcmd' for the second part of the script that will
#  - Reset environment to defaults
#  - Reset the bootcount
#  - Set bootlimit (if required)
#  - Save the environment
#  - Update the 'linux' partition
#  - Update the 'recovery' partition
#  - Update the 'rootfs' partition
#  - Format the 'update' partition
#  - Format the 'data' partition
uuu fb: ucmd setenv bootcmd "
	env default -a;
	setenv dualboot \${dualboot};
	bootcount reset;
	setenv singlemtdsys \${singlemtdsys};
	${bootlimit_cmd};
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

# Create partition table
uuu "fb[-t 10000]:" ucmd run partition_nand_linux

if [ "${SINGLEMTDSYS}" = true ]; then
	uuu "fb[-t 30000]:" ucmd run ubivolscript
fi

if [ "${DUALBOOT}" = true ]; then
	# Update Linux A
	part_update "${LINUX_NAME}_a" "${INSTALL_LINUX_FILENAME}" 15000
	# Update Linux B
	if [ "${INSTALL_DUALBOOT}" = true ]; then
		part_update "${LINUX_NAME}_b" "${INSTALL_LINUX_FILENAME}" 15000
	fi
	# Update Rootfs A
	part_update "${ROOTFS_NAME}_a" "${INSTALL_ROOTFS_FILENAME}" 90000
	# Update Rootfs B
	if [ "${INSTALL_DUALBOOT}" = true ]; then
		part_update "${ROOTFS_NAME}_b" "${INSTALL_ROOTFS_FILENAME}" 90000
	fi
else
	# Update Linux
	part_update "${LINUX_NAME}" "${INSTALL_LINUX_FILENAME}" 15000
	# Update Recovery
	part_update "${RECOVERY_NAME}" "${INSTALL_RECOVERY_FILENAME}" 20000
	# Update Rootfs
	part_update "${ROOTFS_NAME}" "${INSTALL_ROOTFS_FILENAME}" 120000
fi

if [ "${SINGLEMTDSYS}" != true ]; then
	if [ "${DUALBOOT}" != true ]; then
		format_part "${UPDATE_NAME}"
	fi
	format_part "${DATA_NAME}"
fi

# Set the rootfstype if squashfs
if [ "${SQUASHFS}" = true ]; then
	uuu fb: ucmd setenv rootfstype squashfs
fi

uuu fb: ucmd saveenv

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
