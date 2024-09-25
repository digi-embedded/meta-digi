# Copyright (C) 2019, STMicroelectronics - All Rights Reserved
SUMMARY = "Create package containing MobileNetV1 models"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/Apache-2.0;md5=89aea4e17d99a7cacdbeed46a0096b10"

#NBG model files need to be pre-compiled using STM32AI-MPU offline compiler tool.
#This package contain an example compiled for gcnano driver 6.4.13

SRC_URI = "	file://mobilenet_v3_large_100_224_quant.nb \
			file://labels_mobilenet_nbg.txt "

S = "${WORKDIR}"

do_configure[noexec] = "1"
do_compile[noexec] = "1"

do_install() {
	install -d ${D}${prefix}/local/demo-ai/image-classification/models/mobilenet
	install -d ${D}${prefix}/local/demo-ai/image-classification/models/mobilenet/testdata

	# install mobilenet models
	install -m 0644 ${S}/labels_mobilenet_nbg.txt ${D}${prefix}/local/demo-ai/image-classification/models/mobilenet/labels_mobilenet_v3_nbg.txt
	install -m 0644 ${S}/*.nb 					  ${D}${prefix}/local/demo-ai/image-classification/models/mobilenet/
}

FILES:${PN} += "${prefix}/local/"
