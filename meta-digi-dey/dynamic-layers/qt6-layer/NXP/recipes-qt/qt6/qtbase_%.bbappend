# Copyright (C) 2013 Eric Bénard - Eukréa Electromatique
# Copyright (C) 2016 Freescale Semiconductor
# Copyright (C) 2016, 2017 O.S. Systems Software LTDA.
# Copyright (C) 2017-2018 NXP

PACKAGECONFIG_GRAPHICS:imxpxp = " \
    gles2"
PACKAGECONFIG_GRAPHICS:imxgpu2d = " \
    ${@bb.utils.contains('DISTRO_FEATURES', 'x11', ' gl', '', d)} \
    ${PACKAGECONFIG_GRAPHICS_IMX_GPU}"
PACKAGECONFIG_GRAPHICS:imxgpu3d = " \
    gles2 \
    ${PACKAGECONFIG_GRAPHICS_IMX_GPU}"
PACKAGECONFIG_GRAPHICS_IMX_GPU = ""
PACKAGECONFIG_GRAPHICS_IMX_GPU:mx8-nxp-bsp = " \
    gbm kms"

PACKAGECONFIG_GRAPHICS:use-mainline-bsp ?= " \
    gles2 gbm kms"

PACKAGECONFIG += " \
    ${PACKAGECONFIG_PLATFORM}"

PACKAGECONFIG_PLATFORM = ""
PACKAGECONFIG_PLATFORM:imxgpu2d = " \
    no-opengl \
    linuxfb \
    ${PACKAGECONFIG_PLATFORM_EGLFS}"
PACKAGECONFIG_PLATFORM:imxgpu3d = " \
    ${PACKAGECONFIG_PLATFORM_EGLFS}"

PACKAGECONFIG_PLATFORM_EGLFS = ""
PACKAGECONFIG_PLATFORM_EGLFS:imxgpu3d = " \
    ${@bb.utils.contains('DISTRO_FEATURES', 'x11',     '', \
       bb.utils.contains('DISTRO_FEATURES', 'wayland', '', \
                                                       'eglfs', d), d)}"
PACKAGECONFIG_PLATFORM_EGLFS:mx8-nxp-bsp = " \
    eglfs"

PACKAGECONFIG_PLATFORM:use-mainline-bsp = " \
    ${@bb.utils.contains('DISTRO_FEATURES', 'x11', '', 'eglfs', d)}"

PACKAGECONFIG += " \
    ${@bb.utils.contains('DISTRO_FEATURES', 'vulkan', '${PACKAGECONFIG_VULKAN}', '', d)}"
PACKAGECONFIG_VULKAN = ""
PACKAGECONFIG_VULKAN:imxgpu = " \
    ${PACKAGECONFIG_VULKAN_IMX_GPU}"
PACKAGECONFIG_VULKAN_IMX_GPU               = ""
PACKAGECONFIG_VULKAN_IMX_GPU:mx8-nxp-bsp   = "vulkan"
PACKAGECONFIG_VULKAN_IMX_GPU:mx8mm-nxp-bsp = ""

#
# FROM meta-imx
#
FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SRC_URI += "file://qt-${IMX_BACKEND}.sh"

IMX_BACKEND = \
    "${@bb.utils.contains('DISTRO_FEATURES', 'wayland', 'wayland', \
        bb.utils.contains('DISTRO_FEATURES',     'x11', 'x11', \
                                                        '${IMX_BACKEND_FB}', d), d)}"
IMX_BACKEND_FB          = "linuxfb"
IMX_BACKEND_FB:imxgpu3d = "eglfs"

do_install:append () {
    install -d ${D}${sysconfdir}/profile.d/
    install -m 0755 ${WORKDIR}/qt-${IMX_BACKEND}.sh ${D}${sysconfdir}/profile.d/qt.sh
}

do_install:append:ccimx93() {
    if ! grep -qs "^export QMLSCENE_DEVICE=softwarecontext" ${D}${sysconfdir}/profile.d/qt.sh; then
        echo "export QMLSCENE_DEVICE=softwarecontext" >> ${D}${sysconfdir}/profile.d/qt.sh
    fi
}
