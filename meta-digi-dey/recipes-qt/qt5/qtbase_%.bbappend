# Copyright (C) 2013 Eric Bénard - Eukréa Electromatique
# Copyright (C) 2016 O.S. Systems Software LTDA.
# Copyright (C) 2016 Freescale Semiconductor
# Copyright 2017-2018 NXP
# Copyright (C) 2015-2018, Digi International Inc.

FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI_append = " file://qt5.sh"

PACKAGECONFIG_GL_imxpxp   = "gles2"
PACKAGECONFIG_GL_imxgpu2d = "${@bb.utils.contains('DISTRO_FEATURES', 'x11', 'gl', '', d)}"
PACKAGECONFIG_GL_imxgpu3d = "gles2"
PACKAGECONFIG_append = " accessibility examples fontconfig sql-sqlite"
PACKAGECONFIG_append_ccimx6 = " icu"
PACKAGECONFIG_append_ccimx6ul = " linuxfb"

# -eglfs is conditioned on GPU3D with FrameBuffer only
# -no-opengl -linuxfb are conditioned on GPU2D only
# Overwrite the original setting which is in meta-freescale layer
QT_CONFIG_FLAGS_APPEND_imxpxp = "-no-eglfs"
QT_CONFIG_FLAGS_APPEND_imxgpu2d = "-no-eglfs -no-opengl -linuxfb"
QT_CONFIG_FLAGS_APPEND_imxgpu3d = "\
    ${@bb.utils.contains('DISTRO_FEATURES', 'x11', '-no-eglfs', \
        bb.utils.contains('DISTRO_FEATURES', 'wayland', '-no-eglfs', \
            '-eglfs', d), d)}"
QT_CONFIG_FLAGS_append = " ${QT_CONFIG_FLAGS_APPEND} -optimize-size"

PACKAGECONFIG_WAYLAND ?= "${@bb.utils.contains('DISTRO_FEATURES', 'wayland', 'xkbcommon-evdev', '', d)}"
PACKAGECONFIG += "${PACKAGECONFIG_WAYLAND}"

do_install_append() {
	if ls ${D}${libdir}/pkgconfig/Qt5*.pc >/dev/null 2>&1; then
		sed -i 's,-L${STAGING_DIR_HOST}/usr/lib,,' ${D}${libdir}/pkgconfig/Qt5*.pc
	fi
	install -d ${D}${sysconfdir}/profile.d/
	install -m 0755 ${WORKDIR}/qt5.sh ${D}${sysconfdir}/profile.d/qt5.sh
}

FILES_${PN} += "${sysconfdir}/profile.d/qt5.sh"
