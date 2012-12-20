#!/bin/sh
#===============================================================================
#
#  S65bluez-bg.sh
#
#  Copyright (C) 2012 by Digi International Inc.
#  All rights reserved.
#
#  This program is free software; you can redistribute it and/or modify it
#  under the terms of the GNU General Public License version 2 as published by
#  the Free Software Foundation.
#
#
#  !Description: Configure Bluetooth
#
#===============================================================================

set -e

SCRIPTNAME="S65bluez-bg.sh"

ccardwmx28js_bt_init() {
	#
	# Exit if this hardware does not support Bluetooth
	#
	BLUE_TOOTH_VARIANTS="0x02 0x03 0x04"
	MOD_VARIANT="$(cat /sys/kernel/ccardxmx28/mod_variant)"
	if ! echo ${BLUE_TOOTH_VARIANTS} | grep -qs ${MOD_VARIANT}; then
		[ -z "${quietboot}" ] && echo "${SCRIPTNAME}: FAILED (variant ${MOD_VARIANT} does not support bluetooth)"
		exit
	fi

	#
	# Get the Bluetooth MAC address from NVRAM.  Use a default
	# value if the address has not been set.
	#
	
	# TODO: Hardcoded until passed to kernel command line from U-Boot.
	#BTADDR="$(nvram print module btaddr1 | sed 's,btaddr1=,,g')"
	#if [ -z "${BTADDR}" -o "${BTADDR}" = "00:00:00:00:00:00" ]; then
		BTADDR="00:04:F3:FF:FF:BB"
	#fi

	#
	# We need to write the Bluetooth MAC address to ar3kbdaddr.pst in
	# the AR3k firmware directory.  However, we don't want to rewrite the
	# file if it already exists and the address is the same because we
	# don't want to wear out NAND flash. So compare the two and only
	# update the copy on NAND if the address has changed.
	#
	FW_MAC="/lib/firmware/ar3k/1020200/ar3kbdaddr.pst"
	[ -f "${FW_MAC}" ] && [ "$(cat ${FW_MAC})" = "${BTADDR}" ] || echo ${BTADDR} > ${FW_MAC}

	#
	# Start the Bluetooth driver and daemon (D-BUS must already be running)
	#
	BT_DEVICE="/dev/ttyBt"
	BT_DRIVER="ath3k"
	BT_BAUD_RATE="4000000"
	HCIATTACH_OPTIONS="${BT_DEVICE} ${BT_DRIVER} ${BT_BAUD_RATE}"
	HCIATTACH_OPTIONS_115K="${BT_DEVICE} ${BT_DRIVER} 115200"
	TRIES="1"
	MAX_TRIES="11111"
	while !	hciattach ${HCIATTACH_OPTIONS} 1>/dev/null && [	"${TRIES}" != "${MAX_TRIES}" ] ;
	do
		echo "${SCRIPTNAME}: (hciattach), retrying..."
		#
		# If hciattach at 4Kbps doesn't work, then try it at 115K bps
		# just to get the chip working.
		#
		if hciattach ${HCIATTACH_OPTIONS_115K} 1>/dev/null ; then
			#
			# It worked	at 115Kbps.	 The chip should be	recovered now.
			# Kill the daemon so we	can	retry at 4Mbps.
			#
			kill -s	9 `pidof hciattach`
		fi
		TRIES="${TRIES}1"
	done
	if [ "${TRIES}"	== "${MAX_TRIES}" ]	; then
	[ -z "${quietboot}" ] && echo "${SCRIPTNAME}: FAILED (hciattach)"
	exit
	fi
	BT_FILTER_ARGS="-d -x -s -w wlan0"
	if ! abtfilt ${BT_FILTER_ARGS} 1>/dev/null; then
		[ -z "${quietboot}" ] && echo "${SCRIPTNAME}: FAILED (abtfilt)"
		exit
	fi
}



[ -z "${quietboot}" ] && echo "Starting bluetooth services."

# Initialize driver for 'ccardwmx28js'
read -r platform < /sys/kernel/machine/name
[ "${platform}" = "ccardxmx28" ] && ccardwmx28js_bt_init

# Run bluetooth daemon
if hciconfig hci0 up && bluetoothd; then
	:	# No-op
else
	[ -z "${quietboot}" ] && echo "${SCRIPTNAME}: FAILED"
fi

