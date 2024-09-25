# Copyright (C) 2019, STMicroelectronics - All Rights Reserved
SUMMARY = "Create package containing MobileNetV1/2 models"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/Apache-2.0;md5=89aea4e17d99a7cacdbeed46a0096b10"

# These models are not available in the onnx/models repository and integrating the tf2onnx
# converter in the yocto workflow (to convert them from tflite) would require integrating
# tensorflow (not lite) and bazel as well, which is a pretty big undertaking.
# So, for now, we will simply include the converted models manually.

SRC_URI = "	file://mobilenet_v1_0.5_128.onnx \
	   	file://mobilenet_v1_0.5_128_quant.onnx \
		file://mobilenet_v1_1.0_224_quant.onnx \
		file://labels_mobilenet_onnx.txt "

S = "${WORKDIR}"

do_configure[noexec] = "1"
do_compile[noexec] = "1"

do_install() {
	install -d ${D}${prefix}/local/demo-ai/image-classification/models/mobilenet
	install -d ${D}${prefix}/local/demo-ai/image-classification/models/mobilenet/testdata

	# install mobilenet models
	install -m 0644 ${S}/label*.txt ${D}${prefix}/local/demo-ai/image-classification/models/mobilenet/labels_onnx.txt
	install -m 0644 ${S}/*.onnx	 ${D}${prefix}/local/demo-ai/image-classification/models/mobilenet/
}

FILES:${PN} += "${prefix}/local/"
