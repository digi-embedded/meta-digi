# Copyright (C) 2015 Digi International

FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

SRC_URI_append_ccimx6 = " file://qt5.sh"

PACKAGECONFIG_append = " gstreamer"

do_install_append_ccimx6() {
	install -d ${D}${sysconfdir}/profile.d
	install -m 0755 ${WORKDIR}/qt5.sh ${D}${sysconfdir}/profile.d/
}

PACKAGE_ARCH = "${MACHINE_ARCH}"
