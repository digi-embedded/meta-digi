FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SRC_URI += "file://qt-${IMX_BACKEND}.sh"

IMX_BACKEND = \
    "${@bb.utils.contains('DISTRO_FEATURES', 'wayland', 'wayland', \
        bb.utils.contains('DISTRO_FEATURES',     'x11', 'x11', \
                                                        '${IMX_BACKEND_FB}', d), d)}"
IMX_BACKEND_FB          = "linuxfb"
IMX_BACKEND_FB:imxgpu3d = "eglfs"

PACKAGECONFIG_GRAPHICS_IMX_GPU:mx9-nxp-bsp = " \
    gbm kms"

PACKAGECONFIG_PLATFORM = "no-opengl linuxfb"

PACKAGECONFIG_PLATFORM_EGLFS:mx9-nxp-bsp = " \
    eglfs"

PACKAGECONFIG_VULKAN_IMX_GPU:mx8mm-nxp-bsp = "vulkan"
PACKAGECONFIG_VULKAN_IMX_GPU:mx9-nxp-bsp   = "vulkan"

do_install:append () {
    install -d ${D}${sysconfdir}/profile.d/
    install -m 0755 ${WORKDIR}/qt-${IMX_BACKEND}.sh ${D}${sysconfdir}/profile.d/qt.sh
}

do_install:append:ccimx93() {
    if ! grep -qs "^export QMLSCENE_DEVICE=softwarecontext" ${D}${sysconfdir}/profile.d/qt.sh; then
        echo "export QMLSCENE_DEVICE=softwarecontext" >> ${D}${sysconfdir}/profile.d/qt.sh
    fi
}
