#!/bin/sh
#===============================================================================
#
#  Copyright (C) 2022 by Digi International Inc.
#  All rights reserved.
#
#  This program is free software; you can redistribute it and/or modify it
#  under the terms of the GNU General Public License version 2 as published by
#  the Free Software Foundation.
#
#
#  Description:
#    Script to install the driver of the usb serial console.
#===============================================================================

# Exit on any error
set -e

# Unload the selected module
unload_module()
{
	if grep -qs "^${1}" /proc/modules; then
		printf "Module ${1} loaded, unloading the module.\n"
		rmmod "${1}"
	fi
}

# Create rule for the Cypress USB driver
create_rule()
{
	RULE_PATH=/etc/udev/rules.d/90-cyusb.rules
	if [ ! -f "$RULE_PATH" ]; then
		printf "Rule \"$RULE_PATH\" doesn't exist, creating a new one.\n"
		printf "# Cypress USB driver for FX2 and FX3 (C) Cypress Semiconductor Corporation / ATR-LABS" | tee ${RULE_PATH} > /dev/null
		printf "# Rules written by V. Radhakrishnan ( rk@atr-labs.com )" | tee -a ${RULE_PATH}  > /dev/null
		printf "# Cypress USB vendor ID = 0x04b4" | tee -a ${RULE_PATH}  > /dev/null
		printf "KERNEL==\"*\", SUBSYSTEM==\"usb\", ENV{DEVTYPE}==\"usb_device\", ACTION==\"add\", ATTR{idVendor}==\"04b4\", MODE=\"666\"" | tee -a ${RULE_PATH}  > /dev/null
		printf "KERNEL==\"*\", SUBSYSTEM==\"usb\", ENV{DEVTYPE}==\"usb_device\", ACTION==\"remove\", TAG==\"cyusb_dev\"" | tee -a ${RULE_PATH}  > /dev/null
	fi
}

# Blacklist the selected module
blacklist()
{
	BLACKLISTH_PATH=/etc/modprobe.d/blacklist.conf
	if [ ! -f "$BLACKLISTH_PATH" ]; then
		printf "File \"$BLACKLISTH_PATH\" doesn't exist, creating a new one.\n"
		printf "blacklist ${1}" | tee ${BLACKLISTH_PATH} > /dev/null
	else
		printf "File \"$BLACKLISTH_PATH\" exists, checking if the rule is already there.\n"
		if grep -Fq "${1}" $BLACKLISTH_PATH; then
			printf "Rule for ${1} found.\n"
		else
			printf "Rule for ${1} not found. Adding ${1} to the blacklist.\n"
			printf "\nblacklist ${1}" | tee -a ${BLACKLISTH_PATH} > /dev/null
		fi
	fi
}

if [ "$(id -u)" != 0 ]; then
	printf "This script should be run with root privileges.\n"
	exit 1
fi

printf "Installing Cypress USB driver.\n"

create_rule
blacklist "cytherm"

unload_module "cytherm"
unload_module "cdc_acm"

printf "Please plug/unplug your usb device to be recognized.\n"
