# Copyright (C) 2013 Digi International.

PRINC := "${@int(PRINC) + 1}"
PR_append = "+${DISTRO}"

# Overwrite DEPENDS to avoid livrsvg dependency
# which brings in gtk+
DEPENDS = "gst-plugins-base libmusicbrainz tremor curl"

PACKAGECONFIG = "${@base_contains('DISTRO_FEATURES', 'x11', 'x11', '', d)}"
PACKAGECONFIG[x11] = ",--without-x,"

EXTRA_OECONF += "--disable-examples --disable-experimental --disable-sdl --disable-cdaudio --disable-directfb \
                 --with-plugins=musicbrainz,wavpack,ivorbis,mpegvideoparse,freeze --disable-vdpau --disable-apexsink \
                 --disable-orc"

EXTRA_OECONF += "\
		--disable-rsvg \
		--disable-bayer \
		--disable-camerabin \
		--disable-cdxaparse \
		--disable-dccp \
		--disable-debugutils \
		--disable-dtmf \
		--disable-dvb \
		--disable-dvdnav \
		--disable-dvdspu \
		--disable-festival \
		--disable-frei0r \
		--disable-librfb \
		--disable-mve \
		--disable-mxf \
		--disable-neon \
		--disable-nsf \
		--disable-pcapparse \
		--disable-rtpmux \
		--disable-siren \
		--disable-vcd \
		--disable-videosignal \
		"
