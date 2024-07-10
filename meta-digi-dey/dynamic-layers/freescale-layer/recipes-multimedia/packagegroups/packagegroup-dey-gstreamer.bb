#
# Copyright (C) 2012-2017, Digi International Inc.
#
SUMMARY = "Gstreamer framework packagegroup for DEY image"

PACKAGE_ARCH = "${MACHINE_ARCH}"
inherit packagegroup

# Per machine gstreamer base packages
MACHINE_GSTREAMER_1_0_PKGS = " \
    gstreamer1.0-meta-audio \
    gstreamer1.0-meta-video \
    gstreamer1.0-plugins-base-meta \
    gstreamer1.0-plugins-good-meta \
"
# Minimal set of gstreamer elements to play a local WEBM video
MACHINE_GSTREAMER_1_0_PKGS:ccimx6ul = " \
    gstreamer1.0-plugins-base-alsa \
    gstreamer1.0-plugins-base-audioconvert \
    gstreamer1.0-plugins-base-audioresample \
    gstreamer1.0-plugins-base-playback \
    gstreamer1.0-plugins-base-typefindfunctions \
    gstreamer1.0-plugins-base-videoconvert \
    gstreamer1.0-plugins-base-videoscale \
    gstreamer1.0-plugins-base-volume \
    gstreamer1.0-plugins-good-pulseaudio \
    gstreamer1.0-plugins-good-video4linux2 \
    gstreamer1.0-plugins-good-videofilter \
    gstreamer1.0-plugins-good-vpx \
    gstreamer1.0-plugins-good-avi \
    gstreamer1.0-plugins-good-jpeg \
"

MACHINE_GSTREAMER_1_0_EXTRA_INSTALL ?= " \
    gstreamer1.0-plugins-bad-meta \
    gstreamer1.0-plugins-ugly-meta \
    gstreamer1.0-rtsp-server-meta \
    gstreamer1.0-libav \
"
MACHINE_GSTREAMER_1_0_EXTRA_INSTALL:ccimx6ul ?= ""

RDEPENDS:${PN} = " \
    ${MACHINE_GSTREAMER_1_0_PKGS} \
    ${MACHINE_GSTREAMER_1_0_EXTRA_INSTALL} \
    ${MACHINE_GSTREAMER_1_0_PLUGIN} \
    ${@bb.utils.contains("MACHINE_GSTREAMER_1_0_PLUGIN", "imx-gst1.0-plugin", "imx-gst1.0-plugin-gplay", "", d)} \
    ${@bb.utils.contains("MACHINE_GSTREAMER_1_0_PLUGIN", "imx-gst1.0-plugin", "imx-gst1.0-plugin-grecorder", "", d)} \
"
