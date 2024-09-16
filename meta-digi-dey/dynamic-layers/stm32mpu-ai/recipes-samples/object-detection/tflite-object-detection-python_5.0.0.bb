# Copyright (C) 2022, STMicroelectronics - All Rights Reserved
SUMMARY = "TensorFlowLite Python Computer Vision object detection application example"
LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/BSD-3-Clause;md5=550794465ba0ec5312d6919e203a55f9"

SRC_URI  =  " file://tflite;subdir=${BPN}-${PV} "

S = "${WORKDIR}/${BPN}-${PV}"

do_configure[noexec] = "1"
do_compile[noexec] = "1"

do_install() {
    install -d ${D}${prefix}/local/demo/application
    install -d ${D}${prefix}/local/demo-ai/object-detection/tflite

    # install applications into the demo launcher
    install -m 0755 ${S}/tflite/200-tflite-object-detection-python.yaml	${D}${prefix}/local/demo/application

    # install application binaries and launcher scripts
    install -m 0755 ${S}/tflite/tflite_object_detection.py                      ${D}${prefix}/local/demo-ai/object-detection/tflite
    install -m 0755 ${S}/tflite/launch_python_object_detection.sh		        ${D}${prefix}/local/demo-ai/object-detection/tflite
    install -m 0755 ${S}/tflite/launch_python_object_detection_testdata.sh		${D}${prefix}/local/demo-ai/object-detection/tflite
}

do_install:append:stm32mp25common(){
    install -m 0755 ${S}/tflite/200-tflite-object-detection-python-mp2.yaml	    ${D}${prefix}/local/demo/application/200-tflite-object-detection-python.yaml
    install -m 0755 ${S}/tflite/launch_python_object_detection_mp2.sh	        ${D}${prefix}/local/demo-ai/object-detection/tflite/launch_python_object_detection.sh
    install -m 0755 ${S}/tflite/launch_python_object_detection_testdata_mp2.sh	${D}${prefix}/local/demo-ai/object-detection/tflite/launch_python_object_detection_testdata.sh
}

FILES:${PN} += "${prefix}/local/"

RDEPENDS:${PN} += " \
	python3-core \
	python3-numpy \
	python3-opencv \
	python3-pillow \
	python3-pygobject \
	python3-tensorflow-lite \
    application-resources \
	bash \
"

RDEPENDS:${PN}:append:stm32mp25common = " tflite-models-yolov4-tiny "
RDEPENDS:${PN}:append:stm32mp1common  = " tflite-models-coco-ssd-mobilenetv1 "
