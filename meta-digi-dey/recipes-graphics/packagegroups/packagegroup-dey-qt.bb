#
# Copyright (C) 2013 Digi International.
#
SUMMARY = "QT packagegroup for DEY image"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/LICENSE;md5=3f40d7994397109285ec7b81fdeb3b58"
PACKAGE_ARCH = "${MACHINE_ARCH}"

inherit packagegroup

RDEPENDS_${PN} = "\
    packagegroup-core-qt-demoapps \
    qt4-plugin-phonon-backend-gstreamer \
    qt4-demos \
"
