PACKAGECONFIG = "${@base_contains('DISTRO_FEATURES', 'x11', 'x11', '', d)}"
PACKAGECONFIG[x11] = ",--disable-gconf --disable-x --disable-xshm --disable-xvideo,"

DEPENDS_no_X := "${@oe_filter_out('gconf', '${DEPENDS}', d)}"
DEPENDS_del := "${@base_contains('DISTRO_FEATURES', 'x11', '${DEPENDS}', '${DEPENDS_no_X}', d)}"

EXTRA_OECONF_del += "\
		 --disable-aalibtest \
		 --disable-audiofx \
		 --disable-cairo \
		 --disable-cutter \
		 --disable-debug \
		 --disable-debugutils \
		 --disable-directsound \
		 --disable-dv1394 \
		 --disable-effectv \
		 --disable-esdtest \
		 --disable-examples \
		 --disable-flac \
		 --disable-goom \
		 --disable-goom2k1 \
		 --disable-gtk-doc \
		 --disable-libdv \
		 --disable-libpng \
		 --disable-osx_audio \
		 --disable-osx_video \
		 --disable-rpath \
		 --disable-shout2test \
		 --disable-spectrum \
		 --disable-speex \
		 --disable-sunaudio \
		 --disable-valgrind \
		"
