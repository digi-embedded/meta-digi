PACKAGECONFIG:remove = "${PACKAGECONFIG_REMOVE}"
PACKAGECONFIG_REMOVE ?= \
    "${@bb.utils.contains('LICENSE_FLAGS_ACCEPTED', 'commercial', '', 'ffmpeg', d)}"

pkg_postinst:${PN}:ccimx6() {
        echo '# Use FSL gstreamer plugin video source' >> $D${sysconfdir}/profile.d/qt6.sh
        echo 'export QT_GSTREAMER_CAMERABIN_VIDEOSRC="imxv4l2src"' >> $D${sysconfdir}/profile.d/qt6.sh
}
