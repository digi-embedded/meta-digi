# Copyright (C) 2011 Freescale Semiconductor
# Copyright (C) 2012 Digi International.
# Released under the MIT license (see COPYING.MIT for the terms)

DESCRIPTION = "Freescale mm codec libs"
SECTION = "multimedia"
LICENSE = "Proprietary"
DEPENDS = "pkgconfig"

LIC_FILES_CHKSUM := "file://${FSL_MMK_EULA_FILE};md5=da9b44d371e7f1f2d2fa1cad0db90ef7"

INC_PR = "r1"

INSANE_SKIP_${PN} = "ldflags"
INSANE_SKIP_${PN}-dev = "ldflags"

SRC_URI = "${DIGI_LOG_MIRROR}/${PN}-${PV}.tar.gz"

do_install () {
    install -d ${D}${libdir}
    install -d ${D}${libdir}/pkgconfig
    install -d ${D}${includedir}/mm_ghdr

    cp -r ${S}/release/lib/* ${D}${libdir}
    cp -r ${S}/ghdr/* ${D}${includedir}/mm_ghdr
    install -m 0644 ${S}/pkgconfig/fsl-mm-core.pc ${D}${libdir}/pkgconfig

	# Fix file permissions
	find ${D} -type f -exec chmod a-x '{}' ';'
}

FILES_${PN} += "${libdir}/*.so*"

PACKAGE_ARCH = "${MACHINE_ARCH}"
