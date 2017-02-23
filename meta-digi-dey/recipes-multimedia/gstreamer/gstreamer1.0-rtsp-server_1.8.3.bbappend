# Copyright 2015-2017, Digi International Inc.

GST_RTSP_EXAMPLES ?= " \
    test-appsrc \
    test-auth \
    test-cgroups \
    test-launch \
    test-mp4 \
    test-multicast \
    test-multicast2 \
    test-netclock \
    test-netclock-client \
    test-ogg \
    test-readme \
    test-record \
    test-record-auth \
    test-sdp \
    test-uri \
    test-video \
    test-video-rtx \
"

PACKAGECONFIG_append = " examples"
PACKAGECONFIG[examples] = "--enable-examples,--disable-examples"

do_install_append() {
	# Install examples
	install -d ${D}${datadir}/${BP}
	for f in ${GST_RTSP_EXAMPLES}; do
		install -m 0755 ${B}/examples/.libs/${f} ${D}${datadir}/${BP}
	done
}

PACKAGES =+ "${PN}-examples-dbg ${PN}-examples"

FILES_${PN}-examples-dbg += "${datadir}/${BP}/.debug"
FILES_${PN}-examples += "${datadir}"
