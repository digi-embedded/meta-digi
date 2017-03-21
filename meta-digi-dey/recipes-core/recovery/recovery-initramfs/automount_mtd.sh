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

UPDATE_MOUNTPOINT="/mnt/update"
PARTITION_NAME="update"

# This will detect if the block device has a update partition
is_update_device() {
	grep -qs update /proc/mtd
}

# This will verify that the requested partition is the update partition
is_update_partition() {
	grep -qs "^${MDEV}:.*\<update\>.*" /proc/mtd
}

if is_update_device; then
	if is_update_partition; then
		# Attach and get UBI device number
		dev_number="$(ubiattach -p /dev/${MDEV} 2>/dev/null | sed -ne 's,.*device number \([0-9]\).*,\1,g;T;p' 2>/dev/null)"
		# Check if volume exists.
		if ubinfo "/dev/ubi${dev_number}" -N "${PARTITION_NAME}" >/dev/null 2>&1; then
			if mkdir -p ${UPDATE_MOUNTPOINT} && ! mountpoint -q ${UPDATE_MOUNTPOINT}; then
				# Mount the volume.
				if ! mount -t ubifs "ubi${dev_number}:${PARTITION_NAME}" "${UPDATE_MOUNTPOINT}"; then
					echo "ERROR: Could not mount '${PARTITION_NAME}' partition"
					rmdir --ignore-fail-on-non-empty ${UPDATE_MOUNTPOINT}
				fi
			fi
		else
			echo "ERROR: Could not mount '${PARTITION_NAME}' partition, volume not found"
			ubidetach -p "/dev/${MDEV}" >/dev/null 2>&1
			rmdir --ignore-fail-on-non-empty ${UPDATE_MOUNTPOINT}
		fi
	fi
	# If it's 'update' device but not partition, just exit
	exit 0
fi
