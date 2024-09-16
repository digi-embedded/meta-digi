# Copyright (C) 2019, STMicroelectronics - All Rights Reserved
SUMMARY = "Create package containing COCO SSD MobileNetV1 models used for the \
application examples"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/Apache-2.0;md5=89aea4e17d99a7cacdbeed46a0096b10"

SRC_URI = " http://storage.googleapis.com/download.tensorflow.org/models/tflite/coco_ssd_mobilenet_v1_1.0_quant_2018_06_29.zip;subdir=${BPN}-${PV}/coco_ssd_mobilenet_v1_1.0_quant;name=coco_ssd_mobilenet_v1_1.0_quant "
SRC_URI[coco_ssd_mobilenet_v1_1.0_quant.md5sum] = "3764f289165250252d2323d718c04d83"
SRC_URI[coco_ssd_mobilenet_v1_1.0_quant.sha256sum] = "a809cd290b4d6a2e8a9d5dad076e0bd695b8091974e0eed1052b480b2f21b6dc"

S = "${WORKDIR}/${BPN}-${PV}"

do_configure[noexec] = "1"
do_compile[noexec] = "1"

do_install() {
    install -d ${D}${prefix}/local/demo-ai/object-detection/models/coco_ssd_mobilenet
    install -d ${D}${prefix}/local/demo-ai/object-detection/models/coco_ssd_mobilenet/testdata

    # install coco ssd mobilenet model
    # label file of the coco ssd mobilenet may be wrong, patch it before installation
    if [ "$(sed -n '/^???/p;q'  ${S}/coco_ssd_mobilenet_v1_1.0_quant/label*.txt)" = "???" ]; then
        # if the first line match '???' string, remove it
        sed -i '1d' ${S}/coco_ssd_mobilenet_v1_1.0_quant/label*.txt
    fi;
    install -m 0644 ${S}/coco_ssd_mobilenet_v1_1.0_quant/label*.txt		${D}${prefix}/local/demo-ai/object-detection/models/coco_ssd_mobilenet/labels_coco_ssd_mobilenet.txt
    install -m 0644 ${S}/coco_ssd_mobilenet_v1_1.0_quant/*.tflite		${D}${prefix}/local/demo-ai/object-detection/models/coco_ssd_mobilenet/coco_ssd_mobilenet.tflite
}

FILES:${PN} += "${prefix}/local/"
