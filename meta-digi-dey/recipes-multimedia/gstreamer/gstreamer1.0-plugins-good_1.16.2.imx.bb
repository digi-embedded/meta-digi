require recipes-multimedia/gstreamer/gstreamer1.0-plugins-good_1.16.2.bb

FILESEXTRAPATHS_prepend := "${COREBASE}/meta/recipes-multimedia/gstreamer/${PN}:"
FILESEXTRAPATHS_prepend := "${COREBASE}/meta/recipes-multimedia/gstreamer/files:"

GST1.0-PLUGINS-GOOD_SRC ?= "gitsm://github.com/nxp-imx/gst-plugins-good.git;protocol=https"
SRCBRANCH = "MM_04.05.07_2011_L5.4.70"
SRCREV = "6005e8199ea19878f269b058ffbbbcaa314472d8"

SRC_URI = " \
    ${GST1.0-PLUGINS-GOOD_SRC};branch=${SRCBRANCH} \
    file://0001-introspection.m4-prefix-pkgconfig-paths-with-PKG_CON.patch \
"

S = "${WORKDIR}/git"

DEPENDS_append = " libdrm"
# This remove "--exclude=autopoint" option from autoreconf argument to avoid
# configure.ac:30: error: required file './ABOUT-NLS' not found
EXTRA_AUTORECONF = ""
PACKAGECONFIG_append_ccimx6ul = " vpx"

COMPATIBLE_MACHINE = "(mx6|mx7|mx8)"
