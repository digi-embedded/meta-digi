#!/bin/sh
#
# Copyright (c) 2017, Digi International Inc.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, you can obtain one at http://mozilla.org/MPL/2.0/.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#

MDEV_AUTOMOUNT_ROOT="/run/media"
UPDATE_MOUNTPOINT="/mnt/update"

DEVICE="$(echo "${MDEV}" | sed -n -e '/^mmc/{s,^\([^p]\+\)p[0-9]\+$,\1,g;T;p}' -e '/^sd/{s,^\([^0-9]\+\)[0-9]\+$,\1,g;T;p}')"
PARTITION="$(echo "${MDEV}" | sed -n -e '/^mmc/{s,^[^p]\+p\([0-9]\+\)$,\1,g;T;p}' -e '/^sd/{s,^[^0-9]\+\([0-9]\+\)$,\1,g;T;p}')"

# This will detect if the block device has a update partition
is_update_device() {
	fdisk -l "/dev/${DEVICE}" | grep -qs update
}

# This will verify that the requested partition is the update partition
is_update_partition() {
	fdisk -l "/dev/${DEVICE}" | sed -ne "s,^[^0-9]*\([0-9]\+\).*\<update\>.*,\1,g;T;p" | grep -qs "${PARTITION}"
}

if is_update_device; then
	if is_update_partition; then
		if mkdir -p ${UPDATE_MOUNTPOINT} && ! mountpoint -q ${UPDATE_MOUNTPOINT}; then
			FSTYPE="$(blkid /dev/${MDEV} | sed -e 's,.*TYPE="\([^"]\+\)".*,\1,g')"
			if ! mount ${FSTYPE:+-t ${FSTYPE}} "/dev/${MDEV}" "${UPDATE_MOUNTPOINT}"; then
				rmdir --ignore-fail-on-non-empty ${UPDATE_MOUNTPOINT}
			fi
		fi
	fi
	# If it's 'update' device but not partition, just exit
	exit 0
fi

case "${ACTION}" in
add)
	# Create mountpoint and mount the mmc device
	if mkdir -p ${MDEV_AUTOMOUNT_ROOT}/${MDEV} && ! mountpoint -q ${MDEV_AUTOMOUNT_ROOT}/${MDEV}; then
		FSTYPE="$(blkid /dev/${MDEV} | sed -e 's,.*TYPE="\([^"]\+\)".*,\1,g')"
		mount -r ${FSTYPE:+-t ${FSTYPE}} /dev/${MDEV} ${MDEV_AUTOMOUNT_ROOT}/${MDEV}
	fi
	;;
remove)
	# Umount and then remove mountpoint
	if grep -q "/dev/${MDEV}[[:blank:]]" /proc/mounts; then
		mdir=$(sed -ne "s,/dev/${MDEV}[[:blank:]]\+\([^[:blank:]]\+\)[[:blank:]].*,\1,g;T;p" /proc/mounts)
		umount "${mdir}"
		rmdir -- "${mdir}" 2>/dev/null
	fi
	;;
esac
