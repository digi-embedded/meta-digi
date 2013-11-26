# Copyright (C) 2013 Digi International.

PRINC := "${@int(PRINC) + 1}"
PR_append = "+${DISTRO}"

FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}-${PV}:"

SRC_URI = " \
    ${FSL_MIRROR}/${PACKAGE_NAME}-${PV}.tar.gz \
    file://0001-gst-fsl-plugin-update-to-DEL-revision.patch \
    file://0002-meta-fsl-arm-fix-segment-fault-in-v4lsink-for-yocto.patch \
    file://0003-meta-fsl-arm-fix-missing-sys-types.h.patch \
    file://0004-meta-fsl-arm-Use-library-s-SONAME-in-dlopen.patch \
    file://0005-gplay_fullscreen.patch \
    file://0006-add-fb-dev.patch \
    file://0007-mfw_v4lsrc_uyvy.patch \
    file://0008-mfw_v4lsec_def_sizes.patch \
    file://0009-mfw_v4lsrc_create_segfault.patch \
    file://0010-gplay_rotate.patch \
    file://0011-mfw_isink-Set-defaults-if-no-vssconfig-found.patch \
    file://0012-gst-fsl-plugin-Clear-framebuffer-of-spurious-content.patch \
    file://0013-mfw_v4lsink-Do-not-ignore-cropping-dimensions.patch \
    file://0014-gplay_next_file.patch \
    file://0015-gplay_repeat.patch \
    file://0016-gst-fsl-plugin-Only-call-MXCFB_SET_OVERLAY_POS-with-.patch \
    file://0017-gst-fsl-plugin-Do-not-blank-the-display-on-device-cl.patch \
    file://0018-undefined-shm_open.patch \
    file://0019-aac-decoder-increase-element-rank.patch \
"

do_install_append() {
    # Remove 'vssconfig' config files as we have a patch that configures
    # the displays on the fly.
    rm -f ${D}${datadir}/vssconfig*
}
