if [ "$USER" == "root" ]; then
    export XDG_RUNTIME_DIR=/run/user/`id -u root`

    export ELM_ENGINE=wayland_shm
    export ECORE_EVAS_ENGINE=wayland_shm
    export ECORE_EVAS_ENGINE=wayland_shm
    export GDK_BACKEND=wayland
    export PULSE_RUNTIME_PATH=/run/user/`id -u root`
    if [ -e $XDG_RUNTIME_DIR/wayland-0 ]; then
        export WAYLAND_DISPLAY=wayland-0
    else
	if [ -e $XDG_RUNTIME_DIR/wayland-1 ]; then
	        export WAYLAND_DISPLAY=wayland-1
	fi
    fi
fi
