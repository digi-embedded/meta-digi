FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

PACKAGECONFIG_GL = " ${@bb.utils.contains('DISTRO_FEATURES', 'opengl', \
                        bb.utils.contains('DISTRO_FEATURES', 'wayland', 'gles2', \
                                                             'gles2 eglfs', d), '', d)}"
PACKAGECONFIG:append = " \
    eglfs \
    examples \
    accessibility \
    ${@bb.utils.contains('DISTRO_FEATURES', 'wayland', '', 'gbm kms', d)} \
"
QT_CONFIG_FLAGS += " -no-sse2 -no-opengles3"

SRC_URI:append = " \
    ${@bb.utils.contains('DISTRO_FEATURES', 'wayland', \
                            'file://qt5-wayland.sh', \
                            'file://qt5-eglfs.sh \
                             file://cursor.json ', d)} \
"
do_install:append () {
    install -d ${D}${sysconfdir}/profile.d/

    if ${@bb.utils.contains('DISTRO_FEATURES', 'wayland', 'true', 'false', d)}; then
        # Wayland backend
        install -m 0755 ${WORKDIR}/qt5-wayland.sh ${D}${sysconfdir}/profile.d/qt5.sh
    else
        # EGLFS backend
        install -d ${D}${datadir}/qt5
        install -m 0755 ${WORKDIR}/qt5-eglfs.sh ${D}/${sysconfdir}/profile.d/qt5.sh
        install -m 0664 ${WORKDIR}/cursor.json ${D}${datadir}/qt5/
    fi
}

FILES:${PN} += " \
    ${sysconfdir}/profile.d/qt5.sh \
    ${@bb.utils.contains('DISTRO_FEATURES', 'wayland', '', '${datadir}/qt5', d)} \
"
