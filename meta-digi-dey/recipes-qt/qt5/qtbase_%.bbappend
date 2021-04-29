# Copyright (C) 2013 Eric Bénard - Eukréa Electromatique
# Copyright (C) 2016 O.S. Systems Software LTDA.
# Copyright (C) 2016 Freescale Semiconductor
# Copyright 2017-2018 NXP
# Copyright (C) 2015-2018, Digi International Inc.

FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

# Digi: we use a custom script per platform, not per backend like NXP does
SRC_URI_append = " \
    file://qt5.sh \
"
SRC_URI_append_imxgpu3d = " \
    ${@bb.utils.contains('DISTRO_FEATURES', 'x11', \
        '', \
        'file://0016-Configure-eglfs-with-egl-pkg-config.patch', d)} \
"

PACKAGECONFIG_DEFAULT_remove_mx8mm = "vulkan"

PACKAGECONFIG += "examples"

PACKAGECONFIG_PLATFORM_IMX_GPU     = ""
PACKAGECONFIG_PLATFORM_IMX_GPU_mx8 = "eglfs"
PACKAGECONFIG_PLATFORM_imxgpu2d += "${PACKAGECONFIG_PLATFORM_IMX_GPU}"
PACKAGECONFIG_PLATFORM_imxgpu3d += "${PACKAGECONFIG_PLATFORM_IMX_GPU}"

PACKAGECONFIG_append_ccimx6 = " icu"
PACKAGECONFIG_append_ccimx6ul = " linuxfb"

PARALLEL_MAKEINST = ""
PARALLEL_MAKE_task-install = "${PARALLEL_MAKEINST}"

do_install_append () {
    if ls ${D}${libdir}/pkgconfig/Qt5*.pc >/dev/null 2>&1; then
        sed -i 's,-L${STAGING_DIR_HOST}/usr/lib,,' ${D}${libdir}/pkgconfig/Qt5*.pc
    fi
    install -d ${D}${sysconfdir}/profile.d/
    install -m 0755 ${WORKDIR}/qt5.sh ${D}${sysconfdir}/profile.d/qt5.sh
}

FILES_${PN} += "${sysconfdir}/profile.d/qt5.sh"
