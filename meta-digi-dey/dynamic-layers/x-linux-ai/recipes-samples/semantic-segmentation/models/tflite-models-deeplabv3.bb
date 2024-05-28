# Copyright (C) 2019, STMicroelectronics - All Rights Reserved
# Model used is based on TFLite model https://tfhub.dev/tensorflow/lite-model/deeplabv3/1/default/1
SUMMARY = "Create package containing deeplabv3 models used for the semantic segmentation application examples"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/Apache-2.0;md5=89aea4e17d99a7cacdbeed46a0096b10"

SRC_URI =   "file://deeplabv3.tflite "
SRC_URI +=  "file://deeplabv3.nb "
SRC_URI +=  "file://labelmap.txt "

S = "${WORKDIR}"

do_configure[noexec] = "1"
do_compile[noexec] = "1"

do_install() {
    install -d ${D}${prefix}/local/demo-ai/semantic-segmentation/models/deeplabv3/
    install -d ${D}${prefix}/local/demo-ai/semantic-segmentation/models/deeplabv3/testdata

    # install deeplabv3 model
    install -m 0644 ${S}/label*.txt                       ${D}${prefix}/local/demo-ai/semantic-segmentation/models/deeplabv3/labelmap.txt
    install -m 0644 ${S}/deeplabv3.tflite                 ${D}${prefix}/local/demo-ai/semantic-segmentation/models/deeplabv3/
    install -m 0644 ${S}/deeplabv3.nb                     ${D}${prefix}/local/demo-ai/semantic-segmentation/models/deeplabv3/
}

FILES:${PN} += "${prefix}/local/"
