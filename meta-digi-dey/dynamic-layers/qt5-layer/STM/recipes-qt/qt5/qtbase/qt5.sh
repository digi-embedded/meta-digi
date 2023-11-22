#!/bin/sh
export QT_QPA_PLATFORM="wayland"

[ -f "/etc/profile.d/weston_profile.sh" ] && return

# Use EGLFS platform plugin for images without Wayland
export QT_QPA_PLATFORM="eglfs" QT_QPA_EGLFS_INTEGRATION="eglfs_viv"
