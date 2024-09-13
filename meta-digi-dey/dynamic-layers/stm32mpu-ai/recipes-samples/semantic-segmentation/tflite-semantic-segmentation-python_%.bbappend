# Copyright (C) 2024, Digi International Inc.

FILESEXTRAPATHS:prepend := "${THISDIR}/../common:"

SRC_URI += " \
    file://scripts/launch_npu_demo.sh \
    file://patches/0001-semantic-segmentation-remove-weston-user-check-from-.patch \
    file://patches/0002-semantic-segmentation-reduce-font-size-for-big-scree.patch \
    file://patches/0003-semantic-segmentation-adapt-sample-for-root-user.patch \
    file://patches/0004-semantic-segmentation-set-camera-preview-to-640x480.patch \
"

do_install:append () {
    # Install the generic launch script.
    install -d ${D}${sysconfdir}/demos/scripts
    install -m 755 ${WORKDIR}/scripts/launch_npu_demo.sh ${D}${sysconfdir}/demos/scripts/
    # Create launch symlink for the demo.
    ln -sf launch_npu_demo.sh ${D}${sysconfdir}/demos/scripts/launch_npu_demo_semantic_segmentation.sh
}

RDEPENDS:${PN} += " \
    libdrm-tests \
"

FILES:${PN} += " \
    ${systemd_unitdir}/demos/scripts/* \
"

# Make this recipe available only for the CCMP25 platform.
COMPATIBLE_MACHINE = "(ccmp25)"
