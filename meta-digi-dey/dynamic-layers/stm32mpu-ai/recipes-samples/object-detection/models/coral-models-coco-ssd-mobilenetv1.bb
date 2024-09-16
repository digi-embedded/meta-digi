# Copyright (C) 2019, STMicroelectronics - All Rights Reserved
SUMMARY = "Create package containing COCO SSD MobileNetV1 models for Edge TPU"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/Apache-2.0;md5=89aea4e17d99a7cacdbeed46a0096b10"

SRC_URI  = " file://coco_ssd_mobilenet_edgetpu.tflite;subdir=${BPN}-${PV}"

S = "${WORKDIR}/${BPN}-${PV}"

do_configure[noexec] = "1"
do_compile[noexec] = "1"

do_install() {
    install -d ${D}${prefix}/local/demo-ai/object-detection/models/coco_ssd_mobilenet

    install -m 0644 ${S}/*_edgetpu.tflite ${D}${prefix}/local/demo-ai/object-detection/models/coco_ssd_mobilenet/
}

FILES:${PN} += "${prefix}/local/"

RDEPENDS:${PN} += " tflite-models-coco-ssd-mobilenetv1 "
