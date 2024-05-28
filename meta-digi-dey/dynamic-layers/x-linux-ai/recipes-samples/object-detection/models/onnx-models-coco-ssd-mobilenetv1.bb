# Copyright (C) 2019, STMicroelectronics - All Rights Reserved
SUMMARY = "Create package containing COCO SSD MobileNetV1 models"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/Apache-2.0;md5=89aea4e17d99a7cacdbeed46a0096b10"

# This model is not available in the onnx/models repository and integrating the tf2onnx
# converter in the yocto workflow (to convert it from tflite) would require integrating
# tensorflow (not lite) and bazel as well, which is a pretty big undertaking.
# So, for now, we will simply include the converted model manually.
SRC_URI = " file://coco_ssd_mobilenet.onnx \
            file://labels_coco_ssd_mobilenet_onnx.txt"

S = "${WORKDIR}"

do_configure[noexec] = "1"
do_compile[noexec] = "1"

do_install() {
	install -d ${D}${prefix}/local/demo-ai/object-detection/models/coco_ssd_mobilenet
	install -d ${D}${prefix}/local/demo-ai/object-detection/models/coco_ssd_mobilenet/testdata

    # install coco ssd mobilenet model
	install -m 0644 ${S}/label*.txt    ${D}${prefix}/local/demo-ai/object-detection/models/coco_ssd_mobilenet/labels_coco_ssd_mobilenet_onnx.txt
	install -m 0644 ${S}/*.onnx        ${D}${prefix}/local/demo-ai/object-detection/models/coco_ssd_mobilenet/
}

FILES:${PN} += "${prefix}/local/"
