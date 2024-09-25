# Copyright (C) 2022, STMicroelectronics - All Rights Reserved
SUMMARY = "onnx Python Computer Vision object detection application example"
LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/BSD-3-Clause;md5=550794465ba0ec5312d6919e203a55f9"

SRC_URI  =  " file://onnx;subdir=${BPN}-${PV} "

S = "${WORKDIR}/${BPN}-${PV}"

do_configure[noexec] = "1"
do_compile[noexec] = "1"

do_install() {
    install -d ${D}${prefix}/local/demo/application
    install -d ${D}${prefix}/local/demo-ai/object-detection/onnx

    # install applications into the demo launcher
    install -m 0755 ${S}/onnx/220-onnx-object-detection-python.yaml	${D}${prefix}/local/demo/application

    # install application binaries and launcher scripts
    install -m 0755 ${S}/onnx/onnx_object_detection.py     ${D}${prefix}/local/demo-ai/object-detection/onnx
    install -m 0755 ${S}/onnx/launch_python*.sh		       ${D}${prefix}/local/demo-ai/object-detection/onnx
}

FILES:${PN} += "${prefix}/local/"

RDEPENDS:${PN} += " \
	python3-core \
	python3-numpy \
	python3-opencv \
	python3-pillow \
	python3-pygobject \
	python3-onnxruntime \
	onnx-models-coco-ssd-mobilenetv1 \
    application-resources \
	bash \
"
