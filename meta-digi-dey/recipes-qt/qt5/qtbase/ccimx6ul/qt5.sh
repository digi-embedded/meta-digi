# Use LINUXFB platform plugin for images without X11
[ -f "/etc/init.d/xserver-nodm" ] || export QT_QPA_PLATFORM="linuxfb"
