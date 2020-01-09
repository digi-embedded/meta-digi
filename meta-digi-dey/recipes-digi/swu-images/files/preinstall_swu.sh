#!/bin/sh
#===============================================================================
#
#  preinstall_swu.sh
#
#  Copyright (C) 2018 by Digi International Inc.
#  All rights reserved.
#
#  This program is free software; you can redistribute it and/or modify it
#  under the terms of the GNU General Public License version 2 as published by
#  the Free Software Foundation.
#
#  !Description: SWUpdate pre-install script to open the virtual mapped device
#
#  SWUpdate calls this script before installing the image.
#
#===============================================================================

# Functions.
#------------------------------------------------------------------------------
# Function - psplash_message
#
# Shows the given message in the psplash screen.
#
# @param ${1}  - Message to show.
#------------------------------------------------------------------------------
psplash_message() {
	echo "MSG ${1}" > /tmp/psplash_fifo
	sleep 0.2
}

#------------------------------------------------------------------------------
# Function - psplash_progress
#
# Sets the psplash progress bar percentage to the given one.
#
# @param ${1}  - Progress percentage.
#------------------------------------------------------------------------------
psplash_progress() {
	echo "PROGRESS ${1}" > /tmp/psplash_fifo
	sleep 0.2
}

#------------------------------------------------------------------------------
# Function - log
#
# Prints the given text in the console.
#
# @param ${1}  - Text to print.
#------------------------------------------------------------------------------
log() {
	echo "[FW UPDATE] ${1}"
}

#------------------------------------------------------------------------------
# Function - log_error
#
# Prints the given text in the console as an error.
#
# @param ${1}  - Error text to print.
#------------------------------------------------------------------------------
log_error() {
	log "[ERROR] ${1}"
	psplash_message "ERROR: ${1}"
	psplash_progress "0"
}

# Main
#------------------------------------------------------------------------------
# Check if encrypted device is already open.
if [ -b /dev/mapper/cryptroot ]; then
	exit 0
fi

rootfs_block="/dev/mmcblk0p$(fdisk -l /dev/mmcblk0 | sed -ne "s,^[^0-9]*\([0-9]\+\).*\<rootfs\>.*,\1,g;T;p")"

# Open LUKS encrypted device
trustfence-tool ${rootfs_block} cryptroot
if [ "$?" != "0" ]; then
	log_error "Error executing the firmware update, cannot open virtual device"
	exit 1
fi

