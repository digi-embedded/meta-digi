#!/bin/sh
#===============================================================================
#
#  Copyright (C) 2024 by Digi International Inc.
#  All rights reserved.
#
#  This program is free software; you can redistribute it and/or modify it
#  under the terms of the GNU General Public License version 2 as published by
#  the Free Software Foundation.
#
#
#  !Description: Launch script for NXP EIQ demos
#
#===============================================================================

DEFAULT_DEMO="dms"
DEMO=${DEFAULT_DEMO}

# Try to extract the demo to launch from the script name.
DEMO_FROM_NAME="$(basename "${0}" | sed -n 's/^launch_eiq_demo_\(.*\)\.sh$/\1/p')"

# Check if the demo to launch was passed as argument.
if [ -n "${1}" ]; then
    DEMO=${1}
elif [ -n "${DEMO_FROM_NAME}" ]; then
    DEMO=${DEMO_FROM_NAME}
fi

# Build demo directory.
DEMO_DIR="/usr/bin/eiq-examples-git/${DEMO}"

# Verify that the demo directory exists.
[ -d "${DEMO_DIR}" ] || { echo "Error: Demo ${DEMO} does not exist"; exit 1; }

# Navigate to the demo folder
cd "${DEMO_DIR}" || exit

# Execute the demo pre-configuring the display settings.
WAYLAND_DISPLAY=/run/wayland-0 DISPLAY=:0.0 XDG_RUNTIME_DIR=/run/user/0 python3 main.py -i /dev/video0 -f -d /usr/lib/libethosu_delegate.so
