#!/bin/sh
#
# Copyright (c) 2017,2018 Digi International Inc.
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

# At this point of the boot (udev script), the system log (syslog) is not
# available yet, so use the kernel log buffer from userspace.
log() {
	printf "<$1>qca65x4: $2\n" >/dev/kmsg
}

# Get the permissions of the filesystem containing the given path
get_filesystem_access() {
	[ -z "${1}" ] && return

	fs_device="$(df ${1} | awk 'NR==2 { print $1 }')"
	fs_access="$(awk -v dev="${fs_device}" '$0 ~ dev { print substr($4,1,2) }' < /proc/mounts)"
	echo ${fs_access}
}

# Get the mount point of the filesystem containing the given path
get_filesystem_mount_point() {
	[ -z "${1}" ] && return

	fs_device="$(df ${1} | awk 'NR==2 { print $1 }')"
	fs_mount_point="$(awk -v dev="${fs_device}" '$0 ~ dev { print $2 }' < /proc/mounts)"
	echo ${fs_mount_point}
}

# Remount the filesystem containing the given path as 'read-write' if it was
# mounted as 'read-only'.
set_filesystem_rw_access() {
	[ -z "${1}" ] && return

	if [ "$(get_filesystem_access ${1})" = "ro" ]; then
		mount -o remount,rw $(get_filesystem_mount_point ${1})
	fi
}

# Do nothing if the wireless node does not exist on the device tree
[ -d "/proc/device-tree/wireless" ] || exit 0

# Do nothing if the module is already loaded
grep -qws 'wlan' /proc/modules && exit 0

FS_ORIGINAL_ACCESS="$(get_filesystem_access ${FIRMWARE_DIR})"

# Create symbolic links to the proper FW files depending on the country region
# Use a sub-shell here to change to firmware directory
(
	cd "${FIRMWARE_DIR}"

	BDATA_SOURCE="bdwlan30_US.bin"
	log "5" "Setting US wireless region"

	# When defined, update the links only if they are wrong so we don't
	# rewrite the internal memory every time we boot
	BDATA_LINK="bdwlan30.bin"
	UTFBDATA_LINK="utfbd30.bin"
	if ([ ! -f "${BDATA_LINK}" ] || ! cmp -s "${BDATA_LINK}" "${BDATA_SOURCE}"); then
		set_filesystem_rw_access ${FIRMWARE_DIR}
		ln -sf "${BDATA_SOURCE}" "${BDATA_LINK}"
		ln -sf "${BDATA_SOURCE}" "${UTFBDATA_LINK}"
	fi
)

# Load the wireless module with the params defined in modprobe.d/qualcomm.conf
# and reduce the console log level to avoid debug messages at boot time
LOGLEVEL="$(sed -ne 's,^kernel.printk[^=]*=[[:blank:]]*\(.*\)$,\1,g;T;p' /etc/sysctl.conf 2>/dev/null)"
[ -n "${LOGLEVEL}" ] && sysctl -q -w kernel.printk="${LOGLEVEL}"
modprobe wlan

# Verify the interface is present
[ -d "/sys/class/net/wlan0" ] || log "3" "[ERROR] Loading wlan module"

# Restore the filesystem with the original access permissions if it has been
# changed inside the script.
if [ "$(get_filesystem_access ${FIRMWARE_DIR})" != "${FS_ORIGINAL_ACCESS}" ]; then
	mount -o remount,${FS_ORIGINAL_ACCESS} $(get_filesystem_mount_point ${FIRMWARE_DIR})
fi
