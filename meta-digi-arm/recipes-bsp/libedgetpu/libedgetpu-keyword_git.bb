SUMMARY = "Edge TPU keyphrase detector"
HOMEPAGE = "https://coral.ai/examples"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://LICENSE;md5=3b83ef96387f14655fc854ddc3c6bd57"

SRCREV = "43a5fd0578c75c9140b4f057de74f2dbac96ceff"
SRC_URI = "git://github.com/google-coral/project-keyword-spotter.git;protocol=https;branch=master"

S = "${WORKDIR}/git"

RDEPENDS:${PN} = "python3-pycoral \
                  python3-pyaudio \
"

do_install() {
    # Install Gstreamer examples
    install -d ${D}/opt/libedgetpu
    install -d ${D}/opt/libedgetpu/keyword
    install -d ${D}/opt/libedgetpu/keyword/models
    install -d ${D}/opt/libedgetpu/keyword/config

    install -m 0555 ${S}/models/* ${D}/opt/libedgetpu/keyword/models/
    install -m 0555 ${S}/run_model.py ${D}/opt/libedgetpu/keyword
    install -m 0555 ${S}/mel_features.py ${D}/opt/libedgetpu/keyword
    install -m 0555 ${S}/model.py ${D}/opt/libedgetpu/keyword
    install -m 0555 ${S}/audio_recorder.py ${D}/opt/libedgetpu/keyword

    install -m 0555 ${S}/config/labels_gc2.raw.txt ${D}/opt/libedgetpu/keyword/config
    install -m 0555 ${S}/config/commands_v2.txt ${D}/opt/libedgetpu/keyword/config
}

FILES:${PN} += "/opt/libedgetpu/keyword/* \
"
