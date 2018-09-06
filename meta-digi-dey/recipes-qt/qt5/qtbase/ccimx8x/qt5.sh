#!/bin/sh
export QT_QPA_PLATFORM="wayland"

# Use EGLFS platform plugin for images without XWayland
[ -f "/etc/profile.d/weston.sh" ] || export QT_QPA_PLATFORM="eglfs"
