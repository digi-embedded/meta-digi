#!/bin/sh
if grep -q devtmpfs /proc/filesystems; then
	# mount the devtmpfs on /dev, if not already done
	LANG=C awk '$2 == "/dev" && ($3 == "devtmpfs") { exit 1 }' /proc/mounts && {
	mount -n -o mode=0755 -t devtmpfs none "/dev"
	}
fi
