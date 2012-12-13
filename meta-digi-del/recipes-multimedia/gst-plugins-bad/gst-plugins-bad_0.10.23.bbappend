# Overwrite DEPENDS to avoid livrsvg dependency
# which brings in gtk+
DEPENDS = "gst-plugins-base libmusicbrainz tremor curl"

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
		--without-x \
		"
