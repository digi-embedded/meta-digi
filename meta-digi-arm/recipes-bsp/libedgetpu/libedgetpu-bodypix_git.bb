SUMMARY = "Edge TPU keyphrase detector"
HOMEPAGE = "https://coral.ai/examples"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://LICENSE;md5=3b83ef96387f14655fc854ddc3c6bd57"

SRCREV = "655a354df5f939602ff6b9da2cbc4d2c78898107"
SRC_URI = "git://github.com/google-coral/project-bodypix.git;protocol=https;branch=master"

S = "${WORKDIR}/git"

RDEPENDS:${PN} = "python3-pycoral \
                  python3-svgwrite \
                  python3-scipy \
                  libusb1 \
"

do_install() {
    # Install Gstreamer examples
    install -d ${D}/opt/libedgetpu
    install -d ${D}/opt/libedgetpu/bodypix
    install -d ${D}/opt/libedgetpu/bodypix/models
    install -d ${D}/opt/libedgetpu/bodypix/posenet_lib
    install -d ${D}/opt/libedgetpu/bodypix/posenet_lib/aarch64

    install -m 0555 ${S}/models/bodypix_mobilenet* ${D}/opt/libedgetpu/bodypix/models/
    install -m 0555 ${S}/bodypix.py ${D}/opt/libedgetpu/bodypix
    install -m 0555 ${S}/gstreamer.py ${D}/opt/libedgetpu/bodypix
    install -m 0555 ${S}/pose_engine.py ${D}/opt/libedgetpu/bodypix

    install -m 0555 ${S}/posenet_lib/aarch64/posenet_decoder.so ${D}/opt/libedgetpu/bodypix/posenet_lib/aarch64
}

FILES:${PN} += "/opt/libedgetpu/bodypix/* \
"

INSANE_SKIP:${PN} += "already-stripped"
