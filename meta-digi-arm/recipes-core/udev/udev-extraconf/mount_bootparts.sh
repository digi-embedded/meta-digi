#!/bin/sh
#===============================================================================
#
#  mount_bootparts.sh
#
#  Copyright (C) 2014 by Digi International Inc.
#  All rights reserved.
#
#  This program is free software; you can redistribute it and/or modify it
#  under the terms of the GNU General Public License version 2 as published by
#  the Free Software Foundation.
#
#  !Description: Attempt to mount boot partitions read-only (called from udev)
#
#===============================================================================

MOUNT="/bin/mount"

# Use 'silent' if util-linux's mount (busybox's does not support that option)
[ "$(readlink ${MOUNT})" = "/bin/mount.util-linux" ] && MOUNT="${MOUNT} -o silent"

MOUNTPOINT="/mnt/${ID_PART_ENTRY_NAME}"
mkdir -p ${MOUNTPOINT}
if ! ${MOUNT} -t auto -r ${DEVNAME} ${MOUNTPOINT}; then
	logger -t udev "mount_bootparts.sh: mount ${DEVNAME} under ${MOUNTPOINT} failed!"
	rmdir --ignore-fail-on-non-empty ${MOUNTPOINT}
fi
