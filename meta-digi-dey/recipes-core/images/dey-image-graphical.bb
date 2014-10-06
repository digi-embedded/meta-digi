#
# Copyright (C) 2012 Digi International.
#
DESCRIPTION = "Graphical image based on SATO, a gnome mobile environment visual style."

PR = "${INC_PR}.0"

IMAGE_FEATURES += " \
    dey-qt \
    package-management \
    x11-base \
    x11-sato \
"

LICENSE = "MIT"

include dey-image-minimal.bb

REQUIRED_DISTRO_FEATURES = "x11"

SOC_IMAGE_INSTALL = ""
SOC_IMAGE_INSTALL_mx5 = "amd-gpu-x11-bin-mx51"
SOC_IMAGE_INSTALL_mx6 = "gpu-viv-bin-mx6q gpu-viv-g2d"

IMAGE_INSTALL += " \
    ${SOC_IMAGE_INSTALL} \
    ${@base_contains("MACHINE_FEATURES", "accel-video", "owl-video", "", d)} \
    pointercal-xinput \
"

# Do not install some of the 'RRECOMMENDS_qt4-demos' to save space:
# 'qt4-demos-doc' for all platforms and 'qt4-examples' for ccardimx28
BAD_RECOMMENDATIONS += "qt4-demos-doc"
BAD_RECOMMENDATIONS_append_ccardimx28 = " qt4-examples"

##
## Create a QT4-capable standalone toolchain.
##
## This is mostly copied from 'meta-toolchain-qt' (not included the
## recipe directly because we do not want to inherit populate_sdk)
##
TOOLCHAIN_HOST_TASK += "nativesdk-packagegroup-qt-toolchain-host"

QT_TOOLS_PREFIX = "${SDKPATHNATIVE}${bindir_nativesdk}"

toolchain_create_sdk_env_script_append() {
    echo 'export OE_QMAKE_CFLAGS="$CFLAGS"' >> $script
    echo 'export OE_QMAKE_CXXFLAGS="$CXXFLAGS"' >> $script
    echo 'export OE_QMAKE_LDFLAGS="$LDFLAGS"' >> $script
    echo 'export OE_QMAKE_CC=$CC' >> $script
    echo 'export OE_QMAKE_CXX=$CXX' >> $script
    echo 'export OE_QMAKE_LINK=$CXX' >> $script
    echo 'export OE_QMAKE_AR=$AR' >> $script
    echo 'export OE_QMAKE_LIBDIR_QT=${SDKTARGETSYSROOT}/${libdir}' >> $script
    echo 'export OE_QMAKE_INCDIR_QT=${SDKTARGETSYSROOT}/${includedir}/qt4' >> $script
    echo 'export OE_QMAKE_MOC=${QT_TOOLS_PREFIX}/moc4' >> $script
    echo 'export OE_QMAKE_UIC=${QT_TOOLS_PREFIX}/uic4' >> $script
    echo 'export OE_QMAKE_UIC3=${QT_TOOLS_PREFIX}/uic34' >> $script
    echo 'export OE_QMAKE_RCC=${QT_TOOLS_PREFIX}/rcc4' >> $script
    echo 'export OE_QMAKE_QDBUSCPP2XML=${QT_TOOLS_PREFIX}/qdbuscpp2xml4' >> $script
    echo 'export OE_QMAKE_QDBUSXML2CPP=${QT_TOOLS_PREFIX}/qdbusxml2cpp4' >> $script
    echo 'export OE_QMAKE_QT_CONFIG=${SDKTARGETSYSROOT}/${datadir}/qt4/mkspecs/qconfig.pri' >> $script
    echo 'export QMAKESPEC=${SDKTARGETSYSROOT}/${datadir}/qt4/mkspecs/linux-g++' >> $script
    echo 'export QT_CONF_PATH=${SDKPATHNATIVE}/${sysconfdir}/qt.conf' >> $script

    # make a symbolic link to mkspecs for compatibility with Qt SDK
    # and Qt Creator
    (cd ${SDK_OUTPUT}/${QT_TOOLS_PREFIX}/..; ln -s ${SDKTARGETSYSROOT}/usr/share/qt4/mkspecs mkspecs;)
}
