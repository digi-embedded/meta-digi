# Copyright (C) 2013 Eric Bénard - Eukréa Electromatique
# Copyright (C) 2016 O.S. Systems Software LTDA.
# Copyright (C) 2016 Freescale Semiconductor
# Copyright 2017-2021 NXP
# Copyright (C) 2015-2018, Digi International Inc.

FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

# Digi: we use a custom script per platform, not per backend like NXP does
SRC_URI:append = " \
    file://qt5.sh \
"
SRC_URI:append:imxgpu3d = " \
    ${@bb.utils.contains('DISTRO_FEATURES', 'x11', \
        '', \
        'file://0016-Configure-eglfs-with-egl-pkg-config.patch', d)} \
"

PACKAGECONFIG_DEFAULT:remove:mx8mm-nxp-bsp = "vulkan"

PACKAGECONFIG += "examples"

PACKAGECONFIG_PLATFORM_IMX_GPU     = ""
PACKAGECONFIG_PLATFORM_IMX_GPU:mx8-nxp-bsp = "eglfs"
PACKAGECONFIG_PLATFORM:imxgpu2d += "${PACKAGECONFIG_PLATFORM_IMX_GPU}"
PACKAGECONFIG_PLATFORM:imxgpu3d += "${PACKAGECONFIG_PLATFORM_IMX_GPU}"

PACKAGECONFIG:append:ccimx6 = " icu"
PACKAGECONFIG:append:ccimx6ul = " linuxfb"

PARALLEL_MAKEINST = ""
PARALLEL_MAKE:task-install = "${PARALLEL_MAKEINST}"

do_install:append () {
    if ls ${D}${libdir}/pkgconfig/Qt5*.pc >/dev/null 2>&1; then
        sed -i 's,-L${STAGING_DIR_HOST}/usr/lib,,' ${D}${libdir}/pkgconfig/Qt5*.pc
    fi
    install -d ${D}${sysconfdir}/profile.d/
    install -m 0755 ${WORKDIR}/qt5.sh ${D}${sysconfdir}/profile.d/qt5.sh
}

FILES:${PN} += "${sysconfdir}/profile.d/qt5.sh"
