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

VIRTUAL_RUNTIME_gst-plugins-base = "\
	gst-plugins-base \
	gst-plugins-base-playbin \
	gst-plugins-base-alsa \
	gst-plugins-base-encodebin \
	gst-plugins-base-decodebin \
	gst-plugins-base-decodebin2 \
	"

VIRTUAL_RUNTIME_gst-plugins-good = "\
	gst-plugins-good \
	"

VIRTUAL_RUNTIME_gst-plugins-bad = "\
	gst-plugins-bad \
	"

VIRTUAL_RUNTIME_gst-plugins-ugly = "\
	gst-plugins-ugly \
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
