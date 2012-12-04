# Copyright (C) 2012 Digi International.

PR_append_del = "+${DISTRO}.0"

# live555 does not currently compile, this allows to remove it as a dependency.
VIRTUAL-RUNTIME_streaming_framework = ""
#VIRTUAL-RUNTIME_streaming_framework = "live555"

# Compiling with fontconfig also fails, this allows to remove the dependency
VIRTUAL-RUNTIME_fonts_management = ""
#VIRTUAL-RUNTIME_fonts_management = "fontconfig"

DEPENDS_del = "libdvdread libtheora ffmpeg zlib libpng jpeg liba52 freetype alsa-lib lzo ncurses lame libass \
           ${@base_conditional('ENTERPRISE_DISTRO', '1', '', 'libmad liba52 lame', d)}"
DEPENDS_del += "${VIRTUAL-RUNTIME_streaming_framework}"
DEPENDS_del += "${VIRTUAL-RUNTIME_fonts_management}"
DEPENDS_del += "${@base_contains('DISTRO_FEATURES', 'x11', 'libvpx virtual/libsdl xsp libxv virtual/libx11', '', d)}"

EXTRA_LIBS = " -lstdc++ -lvorbis"
EXTRA_LIBS += "${@base_contains('DISTRO_FEATURES', 'x11', ' -lXext -lX11', '', d)}"
EXTRA_LIBS += "${@base_conditional('VIRTUAL-RUNTIME_streaming_framework', 'live555', ' -lBasicUsageEnvironment -lUsageEnvironment -lgroupsock -lliveMedia', '', d)}"

EXTRA_OECONF_del = " \
	--prefix=/usr \
	--mandir=${mandir} \
	--target=${SIMPLE_TARGET_SYS} \
	\
	--disable-lirc \
	--disable-lircc \
	--disable-joystick \
	--disable-vm \
	--disable-xf86keysym \
	--enable-tv \
	--disable-tv-v4l1 \
	--enable-tv-v4l2 \
	--disable-tv-bsdbt848 \
	--enable-rtc \
	--enable-networking \
	--disable-smb \
	--enable-live \
	--disable-dvdnav \
	--enable-dvdread \
	--disable-dvdread-internal \
	--disable-libdvdcss-internal \
	--disable-cdparanoia \
	--enable-freetype \
	--enable-sortsub \
	--disable-fribidi \
	--disable-enca \
	--disable-ftp \
	--disable-vstream \
	\
	--disable-gif \
	--enable-png \
	--enable-jpeg \
	--disable-libcdio \
	--disable-qtx \
	--disable-xanim \
	--disable-real \
	--disable-xvid \
	\
	--disable-speex \
	--enable-theora \
	--disable-ladspa \
	--disable-libdv \
	--enable-mad \
	--disable-xmms \
	--disable-musepack \
	\
	--disable-gl \
	--disable-vesa \
	--disable-svga \
	--enable-sdl \
	--disable-aa \
	--disable-caca \
	--disable-ggi \
	--disable-ggiwmh \
	--disable-directx \
	--disable-dxr3 \
	--disable-dvb \
	--disable-mga \
	--disable-xmga \
        ${@base_contains('DISTRO_FEATURES', 'x11', '--enable-xv', '', d)} \
	--disable-vm \
	--disable-xinerama \
        ${@base_contains('DISTRO_FEATURES', 'x11', '--enable-x11', '', d)} \
	--enable-fbdev \
	--disable-3dfx \
	--disable-tdfxfb \
	--disable-s3fb \
	--disable-directfb \
	--disable-bl \
	--disable-tdfxvid \
	--disable-tga \
	--disable-pnm \
	--disable-md5sum \
	\
	--enable-alsa \
	--enable-ossaudio \
	--disable-arts \
	--disable-esd \
	--disable-pulse \
	--disable-jack \
	--disable-openal \
	--disable-nas \
	--disable-sgiaudio \
	--disable-sunaudio \
	--disable-win32waveout \
	--enable-select \
	--enable-libass \
	\
        --extra-libs='${EXTRA_LIBS}' \
"

EXTRA_OECONF_del += " \
     ${@base_conditional('VIRTUAL-RUNTIME_fonts_management', 'fontconfig', '', '--disable-fontconfig', d)} \
     ${@base_conditional('VIRTUAL-RUNTIME_streaming_framework', 'live555', '', '--disable-live', d)} \
"

