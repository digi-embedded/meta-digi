# Copyright (C) 2015 Digi International

FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

SRC_URI_append = " file://qt5.sh"

PACKAGECONFIG_GL_mx6ul = "gles2"
QT_CONFIG_FLAGS_append_mx6ul = "${@base_contains('DISTRO_FEATURES', 'x11', ' -no-eglfs', ' -eglfs', d)}"

PACKAGECONFIG_append = " accessibility examples icu linuxfb sql-sqlite"

do_install_append() {
	install -d ${D}${sysconfdir}/profile.d
	install -m 0755 ${WORKDIR}/qt5.sh ${D}${sysconfdir}/profile.d/
}

PACKAGE_ARCH = "${MACHINE_ARCH}"
