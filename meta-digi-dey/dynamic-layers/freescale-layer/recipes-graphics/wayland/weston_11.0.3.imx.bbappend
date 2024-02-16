# Copyright (C) 2024 Digi International Inc.

SRC_URI += " \
    file://0001-Restore-wl_shell-to-weston-11.patch \
    file://0002-Revert-libweston-libinput-device-Enable-Set-pointer-.patch \
"

EXTRA_OEMESON += "-Ddeprecated-wl-shell=true"
