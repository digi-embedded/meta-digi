#
# Copyright (C) 2012 Digi International.
#
DESCRIPTION = "Gstreamer framework task for DEL image"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/LICENSE;md5=3f40d7994397109285ec7b81fdeb3b58"
PACKAGE_ARCH = "${MACHINE_ARCH}"
ALLOW_EMPTY = "1"
PR = "r0"

PACKAGES = "\
	task-del-gstreamer \
	task-del-gstreamer-dbg \
	task-del-gstreamer-dev \
    "

VIRTUAL_RUNTIME_gst-fsl-plugin = "\
	gst-fsl-plugin \
	gst-fsl-plugin-gplay \
	"

# See http://gstreamer.freedesktop.org/data/doc/gstreamer/head/gst-plugins-base-plugins/html/

PACKAGE_gst-plugins-base-core = "\
	gst-plugins-base \
	gst-plugins-base-adder \
	gst-plugins-base-app \
	gst-plugins-base-tcp \
	gst-plugins-base-gdp \
	gst-plugins-base-videotestsrc \
	gst-plugins-base-meta \
	gst-plugins-base-glib \
	gst-plugins-base-typefindfunctions \
	"
PACKAGE_gst-plugins-base-audio = "\
	gst-plugins-base-audiorate \
	gst-plugins-base-audioconvert \
	gst-plugins-base-audioresample \
	gst-plugins-base-alsa \
	gst-plugins-base-audiotestsrc \
	gst-plugins-base-volume \
	"

PACKAGE_gst-plugins-base-ogg-framework = "\
	gst-plugins-base-ogg \
	gst-plugins-base-theora \
	gst-plugins-base-vorbis \
	gst-plugins-base-ivorbisdec \
	"
PACKAGE_gst-plugins-base-conversion = "\
	gst-plugins-base-ffmpegcolorspace \
	gst-plugins-base-videorate \
	gst-plugins-base-videoscale \
	"
PACKAGE_gst-plugins-base-subtitles = "\
	gst-plugins-base-subparse \
	"

PACKAGE_gst-plugins-base-auto = "\
	gst-plugins-base-playbin \
	gst-plugins-base-encodebin \
	gst-plugins-base-decodebin \
	gst-plugins-base-decodebin2 \
	"

VIRTUAL_RUNTIME_gst-plugins-base = "\
	${PACKAGE_gst-plugins-base-core} \
	${PACKAGE_gst-plugins-base-auto} \
	${PACKAGE_gst-plugins-base-conversion} \
	${@base_contains('DISTRO_FEATURES', 'del-audio', '${PACKAGE_gst-plugins-base-audio}', '', d)} \
	"

# See http://gstreamer.freedesktop.org/data/doc/gstreamer/head/gst-plugins-good-plugins/html/


PACKAGE_gst-plugins-good-core = "\
	gst-plugins-good \
	gst-plugins-good-alpha \
	gst-plugins-good-alphacolor \
	gst-plugins-good-apps \
	gst-plugins-good-autodetect \
	gst-plugins-good-glib \
	gst-plugins-good-meta \
	gst-plugins-good-multifile \
	gst-plugins-good-multipart \
				"

PACKAGE_gst-plugins-good-audio = "\
	gst-plugins-good-alaw \
	gst-plugins-good-mulaw \
	gst-plugins-good-audioparsers \
	gst-plugins-good-auparse \
	gst-plugins-good-equalizer \
	gst-plugins-good-interleave \
	gst-plugins-good-level \
	gst-plugins-good-replaygain \
	gst-plugins-good-wavenc \
	gst-plugins-good-wavparse \
				"

PACKAGE_gst-plugins-good-oss = "\
	gst-plugins-good-oss4audio \
	gst-plugins-good-ossaudio \
				"

PACKAGE_gst-plugins-good-graphics = "\
	gst-plugins-good-imagefreeze \
	gst-plugins-good-jpeg \
				"

PACKAGE_gst-plugins-good-video = "\
	gst-plugins-good-avi \
	gst-plugins-good-flv \
	gst-plugins-good-flxdec \
	gst-plugins-good-deinterlace \
	gst-plugins-good-isomp4 \
	gst-plugins-good-matroska \
	gst-plugins-good-smpte \
	gst-plugins-good-video4linux2 \
	gst-plugins-good-videobox \
	gst-plugins-good-videocrop \
	gst-plugins-good-videofilter \
	gst-plugins-good-videomixer \
	gst-plugins-good-y4menc \
				"

PACKAGE_gst-plugins-good-misc = "\
	gst-plugins-good-annodex \
	gst-plugins-good-apetag \
	gst-plugins-good-icydemux \
	gst-plugins-good-id3demux \
	gst-plugins-good-pulse \
	gst-plugins-good-shapewipe \
				"

PACKAGE_gst-plugins-good-streaming = "\
	gst-plugins-good-rtp \
	gst-plugins-good-rtsp \
	gst-plugins-good-souphttpsrc \
	gst-plugins-good-udp \
	gst-plugins-good-rtpmanager \
				"

VIRTUAL_RUNTIME_gst-plugins-good = "\
	${PACKAGE_gst-plugins-good-core} \
	${@base_contains('DISTRO_FEATURES', 'del-audio', '${PACKAGE_gst-plugins-good-audio}', '', d)} \
	${PACKAGE_gst-plugins-good-graphics} \
	${PACKAGE_gst-plugins-good-video} \
	${PACKAGE_gst-plugins-good-streaming} \
	"

# See http://gstreamer.freedesktop.org/data/doc/gstreamer/head/gst-plugins-bad-plugins/html/

PACKAGE_gst-plugins-bad-core = "\
	gst-plugins-bad \
	gst-plugins-bad-apps \
	gst-plugins-bad-glib \
				"

PACKAGE_gst-plugins-bad-misc = "\
	gst-plugins-bad-cog \
	gst-plugins-bad-curl \
	gst-plugins-bad-decklink \
	gst-plugins-bad-fbdevsink \
	gst-plugins-bad-linsys \
	gst-plugins-bad-mpegvideoparse \
	gst-plugins-bad-shm \
	gst-plugins-bad-sndfile \
				"

VIRTUAL_RUNTIME_gst-plugins-bad = "\
	"

# See http://gstreamer.freedesktop.org/data/doc/gstreamer/head/gst-plugins-ugly-plugins/html/

PACKAGE_gst-plugins-ugly-core = "\
	gst-plugins-ugly \
	gst-plugins-ugly-apps \
	gst-plugins-ugly-glib \
	gst-plugins-ugly-meta \
				"

PACKAGE_gst-plugins-ugly-audio = "\
	gst-plugins-ugly-a52dec \
	gst-plugins-ugly-lame \
	gst-plugins-ugly-mad \
	gst-plugins-ugly-mpegaudioparse \
				"
PACKAGE_gst-plugins-ugly-video = "\
	gst-plugins-ugly-asf \
	gst-plugins-ugly-mpeg2dec \
	gst-plugins-ugly-rmdemux \
				"
PACKAGE_gst-plugins-ugly-streaming = "\
	gst-plugins-ugly-mpegstream \
				"
VIRTUAL_RUNTIME_gst-plugins-ugly = "\
	${PACKAGE_gst-plugins-ugly-core} \
	${PACKAGE_gst-plugins-ugly-video} \
	"

RDEPENDS_task-del-gstreamer = "\
    fsl-mm-codeclib \
    fsl-mm-flv-codeclib \
    fsl-mm-mp3enc-codeclib \
    ${VIRTUAL_RUNTIME_gst-fsl-plugin} \
    imx-lib \
    imx-firmware \
    gstreamer \
    ${VIRTUAL_RUNTIME_gst-plugins-base} \
    ${VIRTUAL_RUNTIME_gst-plugins-good} \
    ${VIRTUAL_RUNTIME_gst-plugins-bad} \
    ${VIRTUAL_RUNTIME_gst-plugins-ugly} \
    gst-ffmpeg \
    ${MACHINE_ESSENTIAL_EXTRA_RDEPENDS}"

RRECOMMENDS_task-del-gstreamer = "\
    ${MACHINE_ESSENTIAL_EXTRA_RRECOMMENDS}"
