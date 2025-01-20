FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

# Qt Flags for STM32MP25x - GLES3 support available
QT_CONFIG_FLAGS:remove:stm32mp2common = "-no-opengles3"
QT_CONFIG_FLAGS:append:stm32mp2common = " -opengles3"

SRC_URI:append:class-target = " \
    ${@bb.utils.contains('DISTRO_FEATURES', 'wayland', \
                            'file://qt-wayland.sh', \
                            'file://qt-eglfs.sh \
                             file://cursor.json ', d)} \
"
do_install:append:class-target () {
    install -d ${D}${sysconfdir}/profile.d/

    if ${@bb.utils.contains('DISTRO_FEATURES', 'wayland', 'true', 'false', d)}; then
        # Wayland backend
        install -m 0755 ${WORKDIR}/qt-wayland.sh ${D}${sysconfdir}/profile.d/qt.sh
    else
        # EGLFS backend
        install -d ${D}${datadir}/qt6
        install -m 0755 ${WORKDIR}/qt-eglfs.sh ${D}/${sysconfdir}/profile.d/qt.sh
        install -m 0664 ${WORKDIR}/cursor.json ${D}${datadir}/qt6/
    fi
}

FILES:${PN}:append:class-target = " \
    ${sysconfdir}/profile.d/qt.sh \
    ${@bb.utils.contains('DISTRO_FEATURES', 'wayland', '', '${datadir}/qt6', d)} \
"
