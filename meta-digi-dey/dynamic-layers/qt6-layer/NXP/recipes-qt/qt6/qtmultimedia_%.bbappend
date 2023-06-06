do_install:append() {
if ls ${D}${libdir}/pkgconfig/Qt6*.pc >/dev/null 2>&1; then
    sed -i 's,-L${STAGING_DIR_HOST}/usr/lib,,' ${D}${libdir}/pkgconfig/Qt6*.pc
fi
}

pkg_postinst:${PN}:ccimx6() {
        echo '# Use FSL gstreamer plugin video source' >> $D${sysconfdir}/profile.d/qt6.sh
        echo 'export QT_GSTREAMER_CAMERABIN_VIDEOSRC="imxv4l2src"' >> $D${sysconfdir}/profile.d/qt6.sh
}
