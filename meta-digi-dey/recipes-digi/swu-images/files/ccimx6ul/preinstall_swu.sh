#!/bin/sh
#===============================================================================
#
#  pre-install_swu.sh
#
#  Copyright (C) 2017 by Digi International Inc.
#  All rights reserved.
#
#  This program is free software; you can redistribute it and/or modify it
#  under the terms of the GNU General Public License version 2 as published by
#  the Free Software Foundation.
#
#
#  !Description: SWUpdate pre-install script to remove the encryption flag from
#  rootfs
#
#  SWUpdate calls this script before installing the image.
#
#===============================================================================

# Variables.
#------------------------------------------------------------------------------
ENV_MTDPARTS="mtdparts"

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

#------------------------------------------------------------------------------
# Function - read_uboot_var
#
# Reads the given U-Boot variable.
#
# @param ${1}  - U-Boot variable to read.
# @param ${2}  - Where to store the value of the read variable.
#------------------------------------------------------------------------------
read_uboot_var() {
	eval "${2}=\"$(fw_printenv -n ${1} 2>/dev/null)\""
}

#------------------------------------------------------------------------------
# Function - set_uboot_var
#
# Sets the given U-Boot variable.
#
# @param ${1}  - U-Boot variable to set.
# @param ${2}  - Value to set.
#------------------------------------------------------------------------------
set_uboot_var() {
	fw_setenv ${1} ${2} 2>/dev/null
}

# Main
#------------------------------------------------------------------------------
# Read the mtdparts variable.
read_uboot_var "${ENV_MTDPARTS}" MTDPARTS

# Check if there is any command.
if [ -z "${MTDPARTS}" ]; then
	log_error "No mtdparts found"
	exit 1
fi

# Parse the mtdparts value.
case "${MTDPARTS}" in
	*\(rootfs\)enc*)
		# Remove the flag from the rootfs partition.
		NEW_MTDPARTS=$(echo "${MTDPARTS}" | sed -e "s/(rootfs)enc/(rootfs)/g")
		set_uboot_var "${ENV_MTDPARTS}" "${NEW_MTDPARTS}"
		sync && reboot -f
		;;
	*)
esac

