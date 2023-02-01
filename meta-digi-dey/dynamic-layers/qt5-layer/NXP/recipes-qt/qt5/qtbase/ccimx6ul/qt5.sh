# Use LINUXFB platform plugin for images without X11
[ -f "/etc/init.d/xserver-nodm" ] || export QT_QPA_PLATFORM="linuxfb"

# Use Qt Quick 2D Renderer
export QMLSCENE_DEVICE="softwarecontext"
