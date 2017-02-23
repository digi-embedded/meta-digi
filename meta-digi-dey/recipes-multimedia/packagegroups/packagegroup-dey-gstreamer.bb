#
# Copyright (C) 2012 Digi International.
#
SUMMARY = "Gstreamer framework packagegroup for DEY image"

PACKAGE_ARCH = "${MACHINE_ARCH}"
inherit packagegroup

MACHINE_GSTREAMER_1_0_EXTRA_INSTALL ?= ""
MACHINE_GSTREAMER_1_0_EXTRA_INSTALL_mx6 ?= " \
    gstreamer1.0-plugins-bad-meta \
    gstreamer1.0-plugins-ugly-meta \
    gstreamer1.0-rtsp-server-meta \
"

RDEPENDS_${PN} = " \
    gstreamer1.0-meta-audio \
    gstreamer1.0-meta-video \
    gstreamer1.0-plugins-base-meta \
    gstreamer1.0-plugins-good-meta \
    ${MACHINE_GSTREAMER_1_0_EXTRA_INSTALL} \
    ${MACHINE_GSTREAMER_1_0_PLUGIN} \
"
