# Copyright (C) 2013 Digi International.

PRINC := "${@int(PRINC) + 1}"
PR_append = "+${DISTRO}"

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

DEPENDS += "fsl-mm-flv-codeclib fsl-mm-mp3enc-codeclib"
RDEPENDS_${PN} += "fsl-mm-flv-codeclib fsl-mm-mp3enc-codeclib"

SRC_URI += " \
    file://gst-fsl-plugin-2.0.3-0001-ltmain.patch \
    file://gst-fsl-plugin-2.0.3-0002-gplay_fullscreen.patch \
    file://gst-fsl-plugin-2.0.3-0003-add-fb-dev.patch \
    file://gst-fsl-plugin-2.0.3-0004-mfw_v4lsrc_uyvy.patch \
    file://gst-fsl-plugin-2.0.3-0005-mfw_v4lsec_def_sizes.patch \
    file://gst-fsl-plugin-2.0.3-0006-mfw_v4lsrc_create_segfault.patch \
    file://gst-fsl-plugin-2.0.3-0007-gplay_rotate.patch \
    file://gst-fsl-plugin-2.0.3-0008-mfw_isink-set-defaults-if-no-vssconfig-found.patch \
    file://gst-fsl-plugin-2.0.3-0009-mfw_isink-clear-framebuffer-of-spurious-content.patch \
    file://gst-fsl-plugin-2.0.3-0010-mfw_v4lsink-Do-not-ignore-cropping-dimensions.patch \
    file://gst-fsl-plugin-2.0.3-0011-gplay_next_file.patch \
    file://gst-fsl-plugin-2.0.3-0012-gplay_repeat.patch \
    file://gst-fsl-plugin-2.0.3-0013-Only-call-MXCFB_SET_OVERLAY_POS-with-overlay-framebuffer.patch \
    file://gst-fsl-plugin-2.0.3-0014-Do-not-blank-the-display-on-device-close.patch \
"
