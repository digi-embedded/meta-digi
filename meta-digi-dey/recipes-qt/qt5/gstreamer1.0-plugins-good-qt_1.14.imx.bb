require recipes-multimedia/gstreamer/gstreamer1.0-plugins-good.inc
FILESEXTRAPATHS_prepend := "${BSPDIR}/sources/meta-fsl-bsp-release/imx/meta-sdk/recipes-qt5/qt5/${PN}:"

LIC_FILES_CHKSUM = "file://COPYING;md5=a6f89e2100d9b6cdffcea4f398e37343 \
                    file://common/coverage/coverage-report.pl;beginline=2;endline=17;md5=a4e1830fce078028c8f0974161272607 \
                    file://gst/replaygain/rganalysis.c;beginline=1;endline=23;md5=b60ebefd5b2f5a8e0cab6bfee391a5fe"

GST1.0-PLUGINS-GOOD_SRC ?= "gitsm://github.com/nxp-imx/gst-plugins-good.git;protocol=https"
SRCBRANCH = "MM_04.04.00_1805_L4.9.88_MX8QXP_BETA2"

SRC_URI = " \
    ${GST1.0-PLUGINS-GOOD_SRC};branch=${SRCBRANCH} \
    file://0001-configure.ac-Add-prefix-to-correct-the-QT_PATH.patch \
"
SRCREV = "037e2bf9a152de410623235974c68be21948985a"

# gstgldisplay_viv_fb.h from gst-base is needed by qmlgl plugin
DEPENDS_append = " gstreamer1.0-plugins-base"
# Need libdrm_fourcc.h for DMA buf support in opengl plugins
DEPENDS_append_mx7ulp = " libdrm"
DEPENDS_append_mx8 = " libdrm"

# Qt5 configuratin only support "--disable-qt"
# And in default, it is disabled, need to remove the default setting to enable it.
# Fix: unrecognised options: --disable-sunaudio [unknown-configure-option]
EXTRA_OECONF_remove = "--disable-qt \
                       --disable-sdl --disable-nas --disable-libvisual --disable-xvid --disable-mimic \
                       --disable-pvr --disable-sdltest --disable-wininet --disable-timidity \
                       --disable-linsys --disable-sndio --disable-apexsink \
                       --disable-sunaudio \
"

# The QT_PATH & QT_HOST_PATH which help to access to moc uic rcc tools are incorrect,
# need to passing STAGING_DIR to update the QT PATH
EXTRA_OECONF += "STAGING_DIR=${STAGING_DIR_NATIVE} --disable-introspection"

PACKAGECONFIG += "qt5"

PACKAGECONFIG[qt5] = '--enable-qt \
                      --with-moc="${STAGING_DIR_NATIVE}/usr/bin/qt5/moc" \
                      --with-uic="${STAGING_DIR_NATIVE}/usr/bin/qt5/uic" \
                      --with-rcc="${STAGING_DIR_NATIVE}/usr/bin/qt5/rcc" \
                     ,--disable-qt,qtbase qtdeclarative qtbase-native'

# This remove "--exclude=autopoint" option from autoreconf argument to avoid
# configure.ac:30: error: required file './ABOUT-NLS' not found
EXTRA_AUTORECONF = ""

# remove the duplicate libs except qtsink
do_install_append() {
    rm -rf ${D}/usr
    if [ -e ${WORKDIR}/build/ext/qt/.libs/libgstqmlgl.so ]; then
        mkdir -p ${D}${libdir}/gstreamer-1.0/
        install -m 0755 ${WORKDIR}/build/ext/qt/.libs/libgstqmlgl.so ${D}${libdir}/gstreamer-1.0/
        install -m 0755 ${WORKDIR}/build/ext/qt/.libs/libgstqmlgl.lai ${D}${libdir}/gstreamer-1.0/libgstqmlgl.la
    fi
}

PV = "1.14.0.imx"

S = "${WORKDIR}/git"

# Need qtsink for SoCs that have hardware GPU3D
COMPATIBLE_MACHINE = "(mx6sx|mx6dl|mx6q|mx7ulp|mx8)"
