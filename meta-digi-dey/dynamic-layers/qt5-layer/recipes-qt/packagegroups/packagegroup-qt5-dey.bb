#
# Copyright (C) 2013-2024, Digi International Inc.
#
SUMMARY = "QT packagegroup for DEY image"

PACKAGE_ARCH = "${MACHINE_ARCH}"
inherit packagegroup

# Install Freescale QT demo applications
QT5_APPS = ""
QT5_APPS:imxgpu3d = "${@bb.utils.contains("MACHINE_GSTREAMER_1_0_PLUGIN", "imx-gst1.0-plugin", "imx-qtapplications", "", d)}"

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

QT5_RDEPENDS:imxgpu2d = "${@bb.utils.contains('DISTRO_FEATURES', 'x11','${QT5_RDEPENDS_common}', \
    'qtbase qtbase-plugins', d)}"

QT5_RDEPENDS:imxpxp = "${@bb.utils.contains('DISTRO_FEATURES', 'x11','${QT5_RDEPENDS_common}', \
    'qtbase qtbase-examples qtbase-plugins qtquickcontrols2 qtquickcontrols2-qmlplugins', d)}"

QT5_RDEPENDS:imxgpu3d = " \
    ${QT5_RDEPENDS_common} \
    gstreamer1.0-plugins-good-qt \
"

QT5_RDEPENDS_eglfs = " \
    qtbase                          \
    qtbase-plugins                  \
    qtbase-tools                    \
    qtdeclarative                   \
    qtdeclarative-qmlplugins        \
    qtdeclarative-tools             \
    qtgraphicaleffects-qmlplugins   \
    qtmultimedia                    \
    qtmultimedia-plugins            \
    qtmultimedia-qmlplugins         \
    qtscript                        \
"

QT5_RDEPENDS:ccmp15 = " \
    ${QT5_RDEPENDS_common} \
    ${@bb.utils.contains('DISTRO_FEATURES', 'wayland', '', '${QT5_RDEPENDS_eglfs}', d)} \
"

# Add packagegroup-qt5-webengine to QT5_RDEPENDS and comment out the line below to install qtwebengine to the rootfs.
QT5_RDEPENDS:remove = " packagegroup-qt5-webengine"

RDEPENDS:${PN} += " \
    liberation-fonts \
    ${QT5_RDEPENDS} \
"
