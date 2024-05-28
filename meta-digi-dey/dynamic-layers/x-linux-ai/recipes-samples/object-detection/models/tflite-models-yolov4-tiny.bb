# Copyright (C) 2019, STMicroelectronics - All Rights Reserved
SUMMARY = "Create package containing yolov4_tiny model"
LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/BSD-3-Clause;md5=550794465ba0ec5312d6919e203a55f9"

#yolov4_tiny model weights and config come from https://github.com/AlexeyAB/darknet/#pre-trained-models
#yolov4_tiny model has been generated and exported in tflite using tensorflow python API
#pre-trained yolov4_tiny model has been trained on MS COCO dataset

SRC_URI = " file://yolov4_tiny_416_quant.tflite \
            file://labels_yolov4_tiny.txt "

S = "${WORKDIR}"

do_configure[noexec] = "1"
do_compile[noexec] = "1"

do_install() {
	install -d ${D}${prefix}/local/demo-ai/object-detection/models/yolov4-tiny
	install -d ${D}${prefix}/local/demo-ai/object-detection/models/yolov4-tiny/testdata

    # install coco ssd mobilenet model
	install -m 0644 ${S}/label*.txt    ${D}${prefix}/local/demo-ai/object-detection/models/yolov4-tiny/labels_yolov4_tiny.txt
	install -m 0644 ${S}/*.tflite      ${D}${prefix}/local/demo-ai/object-detection/models/yolov4-tiny/
}

FILES:${PN} += "${prefix}/local/"
