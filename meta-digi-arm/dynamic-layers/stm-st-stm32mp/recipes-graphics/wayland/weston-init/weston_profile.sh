if [ "$USER" == "root" ]; then
    export XDG_RUNTIME_DIR=/run/user/`id -u root`

    export ELM_ENGINE=wayland_shm
    export ECORE_EVAS_ENGINE=wayland_shm
    export ECORE_EVAS_ENGINE=wayland_shm
    export GDK_BACKEND=wayland
    export PULSE_RUNTIME_PATH=/run/user/`id -u root`/pulse
    export USE_PLAYBIN3=1

    # Wait for 10 seconds until a Wayland socket is available
    for i in {1..10}; do
        if [ -e $XDG_RUNTIME_DIR/wayland-0 ]; then
            export WAYLAND_DISPLAY=wayland-0
            break
        elif [ -e $XDG_RUNTIME_DIR/wayland-1 ]; then
            export WAYLAND_DISPLAY=wayland-1
            break
        else
            sleep 1
        fi
    done

    if [ -z "$WAYLAND_DISPLAY" ]; then
        echo "WARNING: No Wayland socket found"
    fi
fi
