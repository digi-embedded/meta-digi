require recipes-multimedia/gstreamer/gstreamer1.0-plugins-good.inc

LIC_FILES_CHKSUM = "file://COPYING;md5=a6f89e2100d9b6cdffcea4f398e37343 \
                    file://common/coverage/coverage-report.pl;beginline=2;endline=17;md5=a4e1830fce078028c8f0974161272607 \
                    file://gst/replaygain/rganalysis.c;beginline=1;endline=23;md5=b60ebefd5b2f5a8e0cab6bfee391a5fe"

GST1.0-PLUGINS-GOOD_SRC ?= "gitsm://github.com/nxp-imx/gst-plugins-good.git;protocol=https"
SRCBRANCH = "MM_04.04.00_1805_L4.9.88_MX8QXP_BETA2"

SRC_URI = " \
    ${GST1.0-PLUGINS-GOOD_SRC};branch=${SRCBRANCH} \
"
SRCREV = "037e2bf9a152de410623235974c68be21948985a"

DEPENDS_append = " libdrm"

EXTRA_AUTORECONF = ""
PACKAGECONFIG_append = " vpx"

# Fix: unrecognised options: --disable-sunaudio [unknown-configure-option]
EXTRA_OECONF_remove = " --disable-sunaudio"

PV = "1.14.0.imx"

S = "${WORKDIR}/git"

