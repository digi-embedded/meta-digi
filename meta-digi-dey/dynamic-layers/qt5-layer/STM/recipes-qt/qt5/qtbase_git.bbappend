FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

PACKAGECONFIG_GL = " ${@bb.utils.contains('DISTRO_FEATURES', 'opengl', 'gles2', '', d)} "
PACKAGECONFIG:append = " eglfs examples accessibility "
QT_CONFIG_FLAGS += " -no-sse2 -no-opengles3"

SRC_URI:append = " \
    file://qt5.sh \
"
do_install:append () {
    install -d ${D}${sysconfdir}/profile.d/
    install -m 0755 ${WORKDIR}/qt5.sh ${D}${sysconfdir}/profile.d/qt5.sh
}

FILES:${PN} += "${sysconfdir}/profile.d/qt5.sh"
