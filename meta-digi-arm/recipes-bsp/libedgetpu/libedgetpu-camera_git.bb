SUMMARY = "Edge TPU simple camera examples"
HOMEPAGE = "https://coral.ai/examples"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://LICENSE;md5=3b83ef96387f14655fc854ddc3c6bd57"

SRCREV = "19335531f599133e054ec2ddcc31733d24031ba5"
SRC_URI = "git://github.com/google-coral/examples-camera.git;protocol=https;branch=master \
           file://0001-gstreamer-fix-video-sink-for-wayland-images.patch \
           "

S = "${WORKDIR}/git"

inherit gobject-introspection

RDEPENDS:${PN} = "python3-pycoral \
                  gstreamer1.0-plugins-base \
"

do_configure() {
    bash download_models.sh
}

do_install() {
    # Install Gstreamer examples
    install -d ${D}/opt/libedgetpu
    install -d ${D}/opt/libedgetpu/camera
    install -d ${D}/opt/libedgetpu/camera/all_models
    install -d ${D}/opt/libedgetpu/camera/gstreamer
    rm -f ${S}/gstreamer/install_requirements.sh
    install -m 0555 ${S}/all_models/* ${D}/opt/libedgetpu/camera/all_models/
    install -m 0555 ${S}/gstreamer/* ${D}/opt/libedgetpu/camera/gstreamer/
}

FILES:${PN} += "/opt/libedgetpu/camera/* \
"
