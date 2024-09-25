# Copyright (C) 2024, Digi International Inc.

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI:remove = " https://tfhub.dev/google/lite-model/movenet/singlepose/lightning/tflite/int8/4?lite-format=tflite;subdir=${BPN}-${PV}/movenet_singlepose_lightning_quant;name=movenet_singlepose_lightning_quant"
SRC_URI:append = " file://movenet_singlepose_lightning.tflite"

do_install() {
    install -d ${D}${prefix}/local/demo-ai/pose-estimation/models/movenet
    install -d ${D}${prefix}/local/demo-ai/pose-estimation/models/movenet/testdata

    # install movenet model
    install -m 0644 ${WORKDIR}/movenet_singlepose_lightning.tflite		${D}${prefix}/local/demo-ai/pose-estimation/models/movenet/
}
