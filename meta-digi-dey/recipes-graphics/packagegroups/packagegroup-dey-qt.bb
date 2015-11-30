#
# Copyright (C) 2013 Digi International.
#
SUMMARY = "QT packagegroup for DEY image"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/LICENSE;md5=3f40d7994397109285ec7b81fdeb3b58"
PACKAGE_ARCH = "${MACHINE_ARCH}"

inherit packagegroup

QT5_PKS = " \
    qtbase-fonts \
    qtdeclarative-tools \
"

QT5_EXAMPLES = " \
    qt3d-examples \
    qtbase-examples \
    qtdeclarative-examples \
    qtmultimedia-examples \
    qtsvg-examples \
    qtwebengine-examples \
"

QT5_DEMOS = " \
    cinematicexperience \
    fslqtapplications \
    qt5-demo-extrafiles \
    qt5everywheredemo \
    qtsmarthome \
"

RDEPENDS_${PN} += " \
    ${QT5_PKS} \
    ${QT5_DEMOS} \
    ${QT5_EXAMPLES} \
"
