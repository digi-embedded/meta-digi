#
# Copyright (C) 2013 Digi International.
#
SUMMARY = "QT packagegroup for DEY image"

PACKAGE_ARCH = "${MACHINE_ARCH}"
inherit packagegroup

MACHINE_QT5_EXTRA_INSTALL ?= ""
MACHINE_QT5_EXTRA_INSTALL_ccimx6 ?= "qtwebengine-examples"

QT5_PKS = "qtbase-fonts qtserialport"
QT5_PKS_append_ccimx6 = " qtdeclarative-tools"

QT5_EXAMPLES = "qtbase-examples"
QT5_EXAMPLES_append_ccimx6 = " \
    qt3d-examples \
    qtconnectivity-examples \
    qtdeclarative-examples \
    qtmultimedia-examples \
    qtsvg-examples \
"

QT5_DEMOS = ""
QT5_DEMOS_append_ccimx6 = " \
    cinematicexperience \
    qt5-demo-extrafiles \
    qt5everywheredemo \
    qtsmarthome \
"

RDEPENDS_${PN} += " \
    ${QT5_PKS} \
    ${QT5_DEMOS} \
    ${QT5_EXAMPLES} \
    ${MACHINE_QT5_EXTRA_INSTALL} \
"
