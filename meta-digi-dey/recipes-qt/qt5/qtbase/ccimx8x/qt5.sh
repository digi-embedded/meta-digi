#!/bin/sh
export QT_QPA_PLATFORM="wayland"

[ -f "/etc/profile.d/weston.sh" ] && exit 0

export QT_QPA_PLATFORM="xcb"

# Use EGLFS platform plugin for images without XWayland and X11
[ -f "/etc/xserver-nodm/Xserver" ] || export QT_QPA_PLATFORM="eglfs"
