find_wayland_display()
{
    if [ -e "$XDG_RUNTIME_DIR/wayland-0" ]; then
        export WAYLAND_DISPLAY=wayland-0
        return 0
    fi

    if [ -e "$XDG_RUNTIME_DIR/wayland-1" ]; then
        export WAYLAND_DISPLAY=wayland-1
        return 0
    fi

    return 1
}

has_connected_drm_display()
{
    local drm_status
    local drm_status_found=false
    local drm_state

    for drm_status in /sys/class/drm/card*-*/status; do
        [ -e "$drm_status" ] || continue

        drm_status_found=true
        read -r drm_state < "$drm_status" || continue
        if [ "$drm_state" = "connected" ]; then
            return 0
        fi
    done

    [ "$drm_status_found" = "false" ]
}

if [ "$USER" = "root" ]; then
    export XDG_RUNTIME_DIR=/run/user/`id -u root`

    export ELM_ENGINE=wayland_shm
    export ECORE_EVAS_ENGINE=wayland_shm
    export ECORE_EVAS_ENGINE=wayland_shm
    export GDK_BACKEND=wayland
    export PULSE_RUNTIME_PATH=/run/user/`id -u root`/pulse
    export USE_PLAYBIN3=1

    if ! find_wayland_display && has_connected_drm_display; then
        # Weston may still be creating its socket after the login prompt
        # appears, but do not delay headless serial logins.
        for i in {1..10}; do
            sleep 1
            find_wayland_display && break
        done
    fi
    [ -z "$WAYLAND_DISPLAY" ] && echo "WARNING: No Wayland socket found"
fi

unset -f find_wayland_display
unset -f has_connected_drm_display
