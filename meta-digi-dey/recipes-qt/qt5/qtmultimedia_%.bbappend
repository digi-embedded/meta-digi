# Copyright (C) 2015 Digi International

PACKAGECONFIG_append = " gstreamer"

pkg_postinst_${PN}_ccimx6() {
	mkdir -p $D${sysconfdir}/profile.d
	echo '# Use FSL gstreamer plugin video source' >> $D${sysconfdir}/profile.d/qt5.sh
	echo 'export QT_GSTREAMER_CAMERABIN_VIDEOSRC="imxv4l2src"' >> $D${sysconfdir}/profile.d/qt5.sh
}

PACKAGE_ARCH = "${MACHINE_ARCH}"
