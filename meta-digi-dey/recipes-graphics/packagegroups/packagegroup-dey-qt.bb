#
# Copyright (C) 2013-2018, Digi International Inc.
#
SUMMARY = "QT packagegroup for DEY image"

PACKAGE_ARCH = "${MACHINE_ARCH}"
inherit packagegroup

# Install Freescale QT demo applications
QT5_APPS = ""
QT5_APPS_imxgpu3d = "${@bb.utils.contains("MACHINE_GSTREAMER_1_0_PLUGIN", "imx-gst1.0-plugin", "imx-qtapplications", "", d)}"

# Install fonts
QT5_FONTS = "ttf-dejavu-common ttf-dejavu-sans ttf-dejavu-sans-mono ttf-dejavu-serif "

# Install Freescale QT demo applications for X11 backend only
MACHINE_QT5_MULTIMEDIA_APPS = ""
QT5_RDEPENDS = ""
QT5_RDEPENDS_common = " \
    packagegroup-qt5-demos \
    ${QT5_FONTS} \
    ${QT5_APPS} \
    ${@bb.utils.contains('DISTRO_FEATURES', 'x11', 'libxkbcommon', '', d)} \
    ${@bb.utils.contains('DISTRO_FEATURES', 'wayland', 'qtwayland qtwayland-plugins', '', d)}\
"

QT5_RDEPENDS_imxgpu2d = "${@bb.utils.contains('DISTRO_FEATURES', 'x11','${QT5_RDEPENDS_common}', \
    'qtbase qtbase-plugins', d)}"

QT5_RDEPENDS_imxpxp = "${@bb.utils.contains('DISTRO_FEATURES', 'x11','${QT5_RDEPENDS_common}', \
    'qtbase qtbase-examples qtbase-plugins', d)}"

QT5_RDEPENDS_imxgpu3d = " \
    ${QT5_RDEPENDS_common} \
    gstreamer1.0-plugins-good-qt \
"

# Add packagegroup-qt5-webengine to QT5_RDEPENDS_mx6 and comment out the line below to install qtwebengine to the rootfs.
QT5_RDEPENDS_remove = " packagegroup-qt5-webengine"

RDEPENDS_${PN} += " \
    liberation-fonts \
    ${QT5_RDEPENDS} \
"
