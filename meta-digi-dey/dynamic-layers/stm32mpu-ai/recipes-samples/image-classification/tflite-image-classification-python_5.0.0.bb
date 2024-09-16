# Copyright (C) 2022, STMicroelectronics - All Rights Reserved
SUMMARY = "TensorFlowLite Python Computer Vision image classification application example"
LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/BSD-3-Clause;md5=550794465ba0ec5312d6919e203a55f9"

SRC_URI  = " file://tflite;subdir=${BPN}-${PV} "

S = "${WORKDIR}/${BPN}-${PV}"

do_configure[noexec] = "1"
do_compile[noexec] = "1"

do_install() {
    install -d ${D}${prefix}/local/demo/application
    install -d ${D}${prefix}/local/demo-ai/image-classification/tflite

    # install applications into the demo launcher
    install -m 0755 ${S}/tflite/100-tflite-image-classification-python.yaml	${D}${prefix}/local/demo/application

    # install application binaries and launcher scripts
    install -m 0755 ${S}/tflite/tflite_image_classification.py         ${D}${prefix}/local/demo-ai/image-classification/tflite
    install -m 0755 ${S}/tflite/launch_python*.sh		               ${D}${prefix}/local/demo-ai/image-classification/tflite
}

do_install:append:stm32mp25common(){
    install -m 0755 ${S}/tflite/100-tflite-image-classification-python-mp2.yaml	${D}${prefix}/local/demo/application/100-tflite-image-classification-python.yaml
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

RDEPENDS:${PN}:append:stm32mp25common = " tflite-models-mobilenetv3 "
RDEPENDS:${PN}:append:stm32mp25common = " nbg-models-mobilenetv3 "
RDEPENDS:${PN}:append:stm32mp1common  = " tflite-models-mobilenetv1 "
