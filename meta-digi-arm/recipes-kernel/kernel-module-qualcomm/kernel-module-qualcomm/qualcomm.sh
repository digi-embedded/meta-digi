#!/bin/sh
#
# Copyright (c) 2023,2024 Digi International Inc.
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

MMC_NODE="##NODE##"
# Lock file to track and prevent to re-run the script when rebinding the MMC node.
LOCKFILE="/tmp/qca65x4.lock"

# At this point of the boot (udev script), the system log (syslog) is not
# available yet, so use the kernel log buffer from userspace.
log() {
	printf "<$1>qca65x4: $2\n" >/dev/kmsg
}

if test -f "$LOCKFILE"; then
	# Script called due to rebinding of mmc. Ignore it and remove the lock file.
	rm $LOCKFILE
	exit 1
fi

# Force re-detection of the mmc node
rebind_mmc_node() {
	DRIVER_NODE=$(find /sys/bus/platform/drivers -name ${MMC_NODE} | xargs dirname 2> /dev/null) || return 1
	echo ${MMC_NODE} > ${DRIVER_NODE}/unbind
	# Give some time to the mmc driver to re-detect the MMC node in order to re-initialize it.
	sleep 2
	echo ${MMC_NODE} > ${DRIVER_NODE}/bind
}

load_and_check() {
	modprobe wlan
	[ -d "/sys/class/net/wlan0" ] && return 0 || return 1
}

# Do nothing if the wireless node does not exist on the device tree
[ -d "/proc/device-tree/wireless" ] || exit 0

# Do nothing if the module is already loaded
grep -qws 'wlan' /proc/modules && exit 0

load_and_check && log "3" "[INFO] wlan module loaded" && exit 0

# If we are here, the load has failed. Retry.
log "3" "[WARN] Loading wlan module failed, retrying..."

# Create a lock file, as rebinding the mmc node will trigger the udev rules
# Do not remove the file at the end, it will be called by the script in the rebind call
touch $LOCKFILE

# Try by re-binding the mmc node.
rebind_mmc_node && load_and_check && log "3" "[INFO] wlan module loaded" && exit 0

log "3" "[ERROR] Loading wlan module"
exit 1
