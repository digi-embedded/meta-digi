# Copyright (C) 2015-2019, Digi International Inc.

PACKAGECONFIG:append = " gstreamer"

pkg_postinst:${PN}:ccimx6() {
	echo '# Use FSL gstreamer plugin video source' >> $D${sysconfdir}/profile.d/qt5.sh
	echo 'export QT_GSTREAMER_CAMERABIN_VIDEOSRC="imxv4l2src"' >> $D${sysconfdir}/profile.d/qt5.sh
}

do_install:append() {
if ls ${D}${libdir}/pkgconfig/Qt5*.pc >/dev/null 2>&1; then
    sed -i 's,-L${STAGING_DIR_HOST}/usr/lib,,' ${D}${libdir}/pkgconfig/Qt5*.pc
fi
}

