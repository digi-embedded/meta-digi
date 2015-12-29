# Copyright (C) 2015 Digi International.

GST_RTSP_EXAMPLES ?= " \
    examples/.libs/test-appsrc \
    examples/.libs/test-auth \
    examples/.libs/test-launch \
    examples/.libs/test-mp4 \
    examples/.libs/test-multicast \
    examples/.libs/test-multicast2 \
    examples/.libs/test-ogg \
    examples/.libs/test-readme \
    examples/.libs/test-sdp \
    examples/.libs/test-uri \
    examples/.libs/test-video \
"

do_install_append() {
	# Install examples
	install -d ${D}${datadir}/${P}
	for f in ${GST_RTSP_EXAMPLES}; do
		install -m 0755 ${B}/${f} ${D}${datadir}/${P}
	done
}

PACKAGES =+ "${PN}-examples-dbg ${PN}-examples"

FILES_${PN}-examples-dbg += "${datadir}/${P}/.debug"
FILES_${PN}-examples += "${datadir}"
