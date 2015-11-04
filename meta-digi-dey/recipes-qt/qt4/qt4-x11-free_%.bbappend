# Copyright (C) 2014 Digi International
#
# Desktop launchers for QT4 demo/example apps
#
# Origin: fsl-gui-extrafiles package (which was removed from meta-fsl-demos)

FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

SRC_URI += " \
    file://qtbrowser.desktop \
    file://qtdemo.desktop \
    file://qthellogles2.desktop \
    file://qtmediaplayer.desktop \
    file://hellogl_es2.png \
    file://qtdemo.png \
    file://qtmediaplayer.png \
    file://webkit.png \
"

# Disable OpenGL for ccardimx28
QT_GLFLAGS_ccardimx28 = "-no-opengl"

do_install_append() {
    install -d ${D}${datadir}/applications ${D}${datadir}/pixmaps
    install -m 0644 ${WORKDIR}/qtbrowser.desktop ${D}${datadir}/applications
    install -m 0644 ${WORKDIR}/qtdemo.desktop ${D}${datadir}/applications
    install -m 0644 ${WORKDIR}/qtmediaplayer.desktop ${D}${datadir}/applications
    install -m 0644 ${WORKDIR}/qtdemo.png ${D}${datadir}/pixmaps
    install -m 0644 ${WORKDIR}/qtmediaplayer.png ${D}${datadir}/pixmaps
    install -m 0644 ${WORKDIR}/webkit.png ${D}${datadir}/pixmaps
}

do_install_append_ccimx6() {
    install -m 0644 ${WORKDIR}/qthellogles2.desktop ${D}${datadir}/applications
    install -m 0644 ${WORKDIR}/hellogl_es2.png ${D}${datadir}/pixmaps
}

FILES_${QT_BASE_NAME}-demos += " \
    ${datadir}/applications/qtbrowser.desktop \
    ${datadir}/applications/qtdemo.desktop \
    ${datadir}/applications/qtmediaplayer.desktop \
    ${datadir}/pixmaps/qtdemo.png \
    ${datadir}/pixmaps/qtmediaplayer.png \
    ${datadir}/pixmaps/webkit.png \
"

FILES_${QT_BASE_NAME}-examples_append_ccimx6 = " \
    ${datadir}/applications/qthellogles2.desktop \
    ${datadir}/pixmaps/hellogl_es2.png \
"

PACKAGE_ARCH = "${MACHINE_ARCH}"
