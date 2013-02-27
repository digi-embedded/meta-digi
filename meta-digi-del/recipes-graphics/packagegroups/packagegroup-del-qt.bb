#
# Copyright (C) 2013 Digi International.
#
DESCRIPTION = "QT packagegroup for DEL image"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/LICENSE;md5=3f40d7994397109285ec7b81fdeb3b58"
PACKAGE_ARCH = "${MACHINE_ARCH}"
ALLOW_EMPTY = "1"
PR = "r0"

inherit packagegroup

# TODO: test the following
# packagegroup-fsl-tools-testapps \
# packagegroup-fsl-tools-benchmark \
# packagegroup-qt-in-use-demos \


RDEPENDS_${PN} = "\
	packagegroup-core-qt-demoapps \
	qt4-plugin-phonon-backend-gstreamer \
	qt4-demos \
	qt4-examples \
	fsl-gui-extrafiles \
    ${MACHINE_ESSENTIAL_EXTRA_RDEPENDS}"

RRECOMMENDS_${PN} = "\
    ${MACHINE_ESSENTIAL_EXTRA_RRECOMMENDS}"
