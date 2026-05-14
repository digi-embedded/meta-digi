# Copyright (C) 2026, Digi International Inc.

FILESEXTRAPATHS:prepend := "${THISDIR}/../common:${THISDIR}/files:"

SRC_URI += " \
    file://scripts/launch_npu_demo.sh \
    file://patches/0001-face-recognition-remove-weston-user-check-from-launc.patch \
    file://patches/0002-face-recognition-add-V4L2SRC-camera-support.patch \
    file://patches/0003-face-recognition-set-camera-preview-to-640x480.patch \
"

do_install:append () {
    # Install the generic launch script.
    install -d ${D}${sysconfdir}/demos/scripts
    install -m 755 ${WORKDIR}/scripts/launch_npu_demo.sh ${D}${sysconfdir}/demos/scripts/
    # Create launch symlink for the demo.
    ln -sf launch_npu_demo.sh ${D}${sysconfdir}/demos/scripts/launch_npu_demo_face_recognition.sh
}

RDEPENDS:${PN} += " \
    libdrm-tests \
    dcmipp-isp-ctrl \
"

FILES:${PN} += " \
    ${systemd_unitdir}/demos/scripts/* \
"
