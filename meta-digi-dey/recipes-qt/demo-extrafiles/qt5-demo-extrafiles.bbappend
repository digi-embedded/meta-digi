# Copyright (C) 2015 Digi International.

do_install_append () {
	# Fix path for OpenGLES example
	sed -i -e '/Exec/{s,hellogl_es2,hellogl2,g}' ${D}${datadir}/applications/hellogl_es2.desktop

	# Remove the desktop launchers of the demo/example applications we do not provide.
	rm -f ${D}${datadir}/applications/qt5basket.desktop ${D}${datadir}/pixmaps/qt5basket.png
	rm -f ${D}${datadir}/applications/qt5nesting.desktop ${D}${datadir}/pixmaps/qt5nesting.png
	rm -f ${D}${datadir}/applications/qt5nmapcarousedemo.desktop ${D}${datadir}/pixmaps/qt5nmapcarousedemo.png
	rm -f ${D}${datadir}/applications/qt5nmapper.desktop ${D}${datadir}/pixmaps/qt5nmapper.png
	rm -f ${D}${datadir}/applications/qt5solarsystem.desktop ${D}${datadir}/pixmaps/qt5solarsystem.png
	rm -f ${D}${datadir}/applications/qtledbillboard.desktop ${D}${datadir}/pixmaps/qtledbillboard.png
	rm -f ${D}${datadir}/applications/qtledcombo.desktop ${D}${datadir}/pixmaps/qtledcombo.png
	rm -f ${D}${datadir}/applications/quitbattery.desktop ${D}${datadir}/pixmaps/quitbattery.png
	rm -f ${D}${datadir}/applications/quitindicators.desktop ${D}${datadir}/pixmaps/quitindicators.png
}
