#!/bin/sh
#===============================================================================
#
#  trustfence-install.sh
#
#  Copyright (C) 2016 by Digi International Inc.
#  All rights reserved.
#
#  This program is free software; you can redistribute it and/or modify it
#  under the terms of the GNU General Public License version 2 as published by
#  the Free Software Foundation.
#
#
#  !Description: Wrapper script for initial deployment of encrypted filesystems
#
#  The script gathers the needed information from the 'trustfence_install'
#  kernel command line parameter with following syntax:
#
#    trustfence_install="source:serverip:filename:partname"
#      source   -> 'tftp' | <block-device>
#      serverip -> <tftp-ip> | ''               (serverip or empty if local)
#      filename -> <image-filename>             (path relative to 'source')
#      partname -> <partition name>             (should match an entry on the
#						 partition table)
#
#  For 'tftp' mode the kernel IP autoconfig may be used to bring the network
#  interface up, with 'ip' kernel parameter. Examples:
#
#    ip=<static-ip>:::<netmask>::eth0:off
#    ip=dhcp
#
#  This script is meant for testing purposes. It's NOT a stable API and may
#  be subject to change.
#
#===============================================================================

set -o pipefail

TF_INSTALL_INFO="${1}"

error() {
	[ "${#}" != "0" ] && printf "\n[ERROR]: %s\n\n" "${1}"
	exit 1
}

# Parse trustfence_install kernel parameter
IFS=":" read SOURCE SERVERIP FILENAME PARTNAME <<_EOF_
${TF_INSTALL_INFO}
_EOF_

# Validate command line arguments
if [ -z "${SOURCE}" ] || [ -z "${FILENAME}" ] || [ -z "${PARTNAME}" ] || { [ "${SOURCE}" = "tftp" ] && [ -z "${SERVERIP}" ]; }; then
	error "wrong 'trustfence_install' parameter: ${TF_INSTALL_INFO}"
fi

# Format partition
mtdindex="$(sed -ne "/\"${PARTNAME}\"$/s,^mtd\([0-9]\):.*,\1,g;T;p" /proc/mtd)"
ubidetach -p /dev/mtd${mtdindex} >/dev/null 2>&1
ubiformat -y /dev/mtd${mtdindex}
UBI_DEVICE="$(ubiattach -p /dev/mtd${mtdindex} | sed -ne 's,.*device number \([0-9]\).*,\1,g;T;p')"
ubimkvol /dev/ubi${UBI_DEVICE} -N "${PARTNAME}" -m

# Install image to the encrypted mapped device
if [ "${SOURCE}" = "tftp" ]; then
	printf "\nInstalling ${FILENAME} from TFTP\n\n"
	FILE=$(basename "$FILENAME")
	tftp -g -l - -r "${FILENAME}" "${SERVERIP}" > ${FILE} || { error "tftp failed"; }
	FILESIZE=$(stat -c%s "$FILE")
	pv -tprebW ${FILE} | ubiupdatevol /dev/ubi${UBI_DEVICE}_0 -s ${FILESIZE} - 2>/dev/null
	rm -f ${FILE}
	if [ "${?}" != "0" ]; then
		error "write ${FILENAME}"
	fi
elif [ -b "${SOURCE}" ]; then
	printf "\nInstalling ${FILENAME} from local media\n\n"
	MOUNTPOINT="/media/$(basename ${SOURCE})"
	FSTYPE="$(blkid ${SOURCE} | sed -e 's,.*TYPE="\([^"]\+\)".*,\1,g')"
	mkdir -p ${MOUNTPOINT}
	mount -r ${FSTYPE:+-t ${FSTYPE}} ${SOURCE} ${MOUNTPOINT}
	FILESIZE=$(stat -c%s "${MOUNTPOINT}/${FILENAME}")
	pv -tprebW ${MOUNTPOINT}/${FILENAME} | ubiupdatevol /dev/ubi${UBI_DEVICE}_0 -s ${FILESIZE} - 2>/dev/null
	if [ "${?}" != "0" ]; then
		error "write ${FILENAME}"
	fi
	umount ${SOURCE}
else
	error "${SOURCE} is neither a block device nor 'tftp'"
fi

echo ""
echo "#######################"
echo "#  Install completed  #"
echo "#######################"
echo ""
