#!/bin/sh
#===============================================================================
#
#  update_images
#
#  Copyright (C) 2023 by Digi International Inc.
#  All rights reserved.
#
#  This program is free software; you can redistribute it and/or modify it
#  under the terms of the GNU General Public License version 2 as published by
#  the Free Software Foundation.
#
#
#  !Description: SWU update images script
#
#===============================================================================

# Sanity check. This script should be always executed with at least one argument.
if [ $# -lt 1 ]; then
	exit 1;
fi

# Called just before installation process starts.
if [ "${1}" = "preinst" ]; then
	:

	# TODO: Execute custom code here. For example:
	# - Mount additional devices/partitions.
	# - Stop services/process before installing files.
fi

# Called just after installation process ends.
if [ "${1}" = "postinst" ]; then
	:

	# TODO: Execute custom code here. For example:
	# - Clean files/directories.
	# - Post-process files.
fi
