#!/bin/sh
export XDG_RUNTIME_DIR="/run/user/0"
export WAYLAND_DISPLAY="wayland-0"
export DBUS_SESSION_BUS_ADDRESS="unix:path=/run/dbus/system_bus_socket"

COMPATIBLE="$(tr '\0' ' ' </proc/device-tree/compatible 2>/dev/null || true)"

case "${COMPATIBLE}" in
    *digi,ccmp15*|*digi,ccmp25*)
        export WL_EGL_GBM_FENCE=0
        ;;
esac

[ -f /etc/default/weston ] && . /etc/default/weston

rm -rf "$XDG_RUNTIME_DIR"
mkdir -p "$XDG_RUNTIME_DIR"
chmod 0700 "$XDG_RUNTIME_DIR"
chown root:root "$XDG_RUNTIME_DIR"

rm -rf /tmp/.X11-unix
mkdir -p /tmp/.X11-unix
chmod 1777 /tmp/.X11-unix

mkdir -p /dev/shm
chmod 1777 /dev/shm

rm -f /tmp/weston.log

weston \
    --backend=drm-backend.so \
    --modules=systemd-notify.so \
    --socket="${WAYLAND_DISPLAY}" \
    --idle-time=0 \
    --log=/tmp/weston.log \
    ${OPTARGS} &

while :; do
    if [ -S "${XDG_RUNTIME_DIR}/${WAYLAND_DISPLAY}" ]; then
        break
    fi
    sleep 0.1
done
sleep 0.1

/etc/connectcore-demo-server start

# wait for server to be ready
until nc -w 1 localhost 9090 </dev/null >/dev/null 2>&1; do
    sleep 0.1
done

exec /usr/bin/chromium \
    --allow-file-access-from-files \
    --enable-features=UseOzonePlatform \
    --enable-wayland-ime \
    --in-process-gpu \
    --incognito \
    --kiosk \
    --no-sandbox \
    --ozone-platform=wayland \
    file:///srv/www/index.html
