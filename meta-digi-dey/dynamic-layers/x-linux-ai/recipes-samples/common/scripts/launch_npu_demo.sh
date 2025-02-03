#!/bin/sh
#===============================================================================
#
#  Copyright (C) 2024,2025, by Digi International Inc.
#  All rights reserved.
#
#  This program is free software; you can redistribute it and/or modify it
#  under the terms of the GNU General Public License version 2 as published by
#  the Free Software Foundation.
#
#
#  !Description: Launch script for ST NPU demos
#
#===============================================================================

# Default demo to launch.
DEFAULT_DEMO="pose_estimation"

# Configure Wayland/Weston settings.
export "DISPLAY=:0.0"
export "XDG_RUNTIME_DIR=/run/user/0"
export "WAYLAND_DISPLAY=wayland-1"

# Wait until Wayland framework is ready.
wait_for_wayland() {
    local count=20
    local wayland_socket="/run/user/0/${WAYLAND_DISPLAY}"

    while [ ! -S "${wayland_socket}" ]; do
        sleep 1
        count=$((count-1))
        if [ "${count}" = 0 ]; then
            return 1
        fi
    done
    return 0
}
[ -d "/usr/share/wayland" ] && wait_for_wayland

# Try to extract the demo to launch from the script name.
DEMO=${DEFAULT_DEMO}
DEMO_FROM_NAME="$(basename "${0}" | sed -n 's/^launch_npu_demo_\(.*\)\.sh$/\1/p')"

# Check if the demo to launch was passed as argument.
if [ -n "${1}" ]; then
    DEMO=${1}
elif [ -n "${DEMO_FROM_NAME}" ]; then
    DEMO=${DEMO_FROM_NAME}
fi

# Build the demo folder name.
DEMO_FOLDER_NAME="$(echo "${DEMO}" | sed 's/_/-/g')"

# Build the demo full path.
DEMO_DIR="/usr/local/x-linux-ai/${DEMO_FOLDER_NAME}"

# Verify that the demo directory exists.
[ -d "${DEMO_DIR}" ] || { echo "Error: Demo ${DEMO} does not exist"; exit 1; }

# Execute the demo.
"${DEMO_DIR}/launch_python_${DEMO}.sh"
