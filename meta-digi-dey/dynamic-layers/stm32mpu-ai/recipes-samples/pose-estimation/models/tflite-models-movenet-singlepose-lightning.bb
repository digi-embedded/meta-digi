# Copyright (C) 2019, STMicroelectronics - All Rights Reserved
SUMMARY = "Create package containing movenet singlepose lightning models used for the \
application examples"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/Apache-2.0;md5=89aea4e17d99a7cacdbeed46a0096b10"

SRC_URI = " https://tfhub.dev/google/lite-model/movenet/singlepose/lightning/tflite/int8/4?lite-format=tflite;subdir=${BPN}-${PV}/movenet_singlepose_lightning_quant;name=movenet_singlepose_lightning_quant "
SRC_URI[movenet_singlepose_lightning_quant.sha256sum] = "cd7cc22fa946e5d146a7b98d496853e1923e22828d3972d579973f27f91bb105"

S = "${WORKDIR}/${BPN}-${PV}"

do_configure[noexec] = "1"
do_compile[noexec] = "1"

do_install() {
    install -d ${D}${prefix}/local/demo-ai/pose-estimation/models/movenet
    install -d ${D}${prefix}/local/demo-ai/pose-estimation/models/movenet/testdata

    #the model is fetched with a bad name -> rename it before installation
    mv ${S}/movenet_singlepose_lightning_quant/4\?lite-format\=tflite ${S}/movenet_singlepose_lightning_quant/movenet_singlepose_lightning.tflite
    # install movenet model
    install -m 0644 ${S}/movenet_singlepose_lightning_quant/*.tflite		${D}${prefix}/local/demo-ai/pose-estimation/models/movenet/
}

FILES:${PN} += "${prefix}/local/"
