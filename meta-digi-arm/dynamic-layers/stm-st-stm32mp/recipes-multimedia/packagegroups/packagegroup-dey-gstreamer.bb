#
# Copyright (C) 2022 Digi International Inc.
#
SUMMARY = "Gstreamer framework packagegroup for DEY image"

PACKAGE_ARCH = "${MACHINE_ARCH}"
inherit packagegroup

# Per machine gstreamer base packages
MACHINE_GSTREAMER_1_0_PKGS = " \
    gstreamer1.0-plugins-base-meta \
    gstreamer1.0-plugins-base \
    gstreamer1.0-plugins-good-meta \
    gstreamer1.0-plugins-bad-meta \
    gstreamer1.0-plugins-ugly-meta \
    gstreamer1.0-libav \
    gstreamer1.0-rtsp-server-meta \
"

# Minimal set of gstreamer elements to play a local WEBM video
MACHINE_GSTREAMER_1_0_PKGS:append = " \
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
    gstreamer1.0-plugins-good-avi \
    gstreamer1.0-plugins-good-jpeg \
"

RDEPENDS:${PN} = " \
    ${MACHINE_GSTREAMER_1_0_PKGS} \
"
