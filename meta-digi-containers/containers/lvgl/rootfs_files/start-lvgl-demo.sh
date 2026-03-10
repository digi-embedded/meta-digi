#!/bin/sh
export XDG_RUNTIME_DIR="/run/user/0"
export WAYLAND_DISPLAY="@WAYLAND_DISPLAY@"

rm -rf "$XDG_RUNTIME_DIR"
mkdir -p "$XDG_RUNTIME_DIR"
chmod 0700 "$XDG_RUNTIME_DIR"
chown root:root "$XDG_RUNTIME_DIR"

rm -rf /tmp/.X11-unix
mkdir -p /tmp/.X11-unix
chmod 1777 /tmp/.X11-unix

rm -f /tmp/weston.log

weston --backend=drm-backend.so --idle-time=0 --log=/tmp/weston.log &

while :; do
    if [ -S "${XDG_RUNTIME_DIR}/${WAYLAND_DISPLAY}" ]; then
        break
    fi
    sleep 0.1
done
sleep 0.1

exec /usr/bin/lvgl-demo