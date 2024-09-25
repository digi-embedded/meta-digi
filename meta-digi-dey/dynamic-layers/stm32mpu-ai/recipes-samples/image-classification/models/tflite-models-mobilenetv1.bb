# Copyright (C) 2019, STMicroelectronics - All Rights Reserved
SUMMARY = "Create package containing MobileNetV1 models"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/Apache-2.0;md5=89aea4e17d99a7cacdbeed46a0096b10"

SRC_URI  = " https://storage.googleapis.com/download.tensorflow.org/models/tflite/mobilenet_v1_1.0_224_quant_and_labels.zip;subdir=${BPN}-${PV}/mobilenet_v1_1.0_224_quant;name=mobilenet_v1_1.0_224_quant "
SRC_URI[mobilenet_v1_1.0_224_quant.md5sum] = "38ac0c626947875bd311ef96c8baab62"
SRC_URI[mobilenet_v1_1.0_224_quant.sha256sum] = "2f8054076cf655e1a73778a49bd8fd0306d32b290b7e576dda9574f00f186c0f"

SRC_URI += " https://storage.googleapis.com/download.tensorflow.org/models/mobilenet_v1_2018_08_02/mobilenet_v1_0.5_128_quant.tgz;subdir=${BPN}-${PV}/mobilenet_v1_0.5_128_quant;name=mobilenet_v1_0.5_128_quant "
SRC_URI[mobilenet_v1_0.5_128_quant.md5sum] = "5cc8484cf04a407fc90993296f3f02db"
SRC_URI[mobilenet_v1_0.5_128_quant.sha256sum] = "0a5b18571d3df4d85a5ac6cb5be829d141dd5855243ea04422ca7d19f730a506"

SRC_URI += " https://storage.googleapis.com/download.tensorflow.org/models/mobilenet_v1_2018_02_22/mobilenet_v1_0.5_128.tgz;subdir=${BPN}-${PV}/mobilenet_v1_0.5_128_float;name=mobilenet_v1_0.5_128_float "
SRC_URI[mobilenet_v1_0.5_128_float.md5sum] = "1950d02e12e2c85613c6f973b1213d1b"
SRC_URI[mobilenet_v1_0.5_128_float.sha256sum] = "5a0def0d844327526385b110cdcaa6428d0828ff6d07515ef25bf3976e049d88"

S = "${WORKDIR}/${BPN}-${PV}"

do_configure[noexec] = "1"
do_compile[noexec] = "1"

do_install() {
    install -d ${D}${prefix}/local/demo-ai/image-classification/models/mobilenet
    install -d ${D}${prefix}/local/demo-ai/image-classification/models/mobilenet/testdata

    # install mobilenet models
    install -m 0644 ${S}/mobilenet_v1_1.0_224_quant/label*.txt ${D}${prefix}/local/demo-ai/image-classification/models/mobilenet/labels.txt
    install -m 0644 ${S}/mobilenet_v1_1.0_224_quant/*.tflite   ${D}${prefix}/local/demo-ai/image-classification/models/mobilenet/
    install -m 0644 ${S}/mobilenet_v1_0.5_128_quant/*.tflite   ${D}${prefix}/local/demo-ai/image-classification/models/mobilenet/
    install -m 0644 ${S}/mobilenet_v1_0.5_128_float/*.tflite   ${D}${prefix}/local/demo-ai/image-classification/models/mobilenet/
}

FILES:${PN} += "${prefix}/local/"
