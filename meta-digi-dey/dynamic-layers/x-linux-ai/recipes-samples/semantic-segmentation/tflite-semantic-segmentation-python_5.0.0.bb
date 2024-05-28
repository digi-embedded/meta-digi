# Copyright (C) 2022, STMicroelectronics - All Rights Reserved
SUMMARY = "TensorFlowLite Python Computer Vision semantic segmentation application example"
LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/BSD-3-Clause;md5=550794465ba0ec5312d6919e203a55f9"

SRC_URI  =  " file://tflite;subdir=${BPN}-${PV} "

# Only compatible with stm32mp25
COMPATIBLE_MACHINE = "stm32mp25common"

S = "${WORKDIR}/${BPN}-${PV}"

do_configure[noexec] = "1"
do_compile[noexec] = "1"

do_install() {
    install -d ${D}${prefix}/local/demo/application
    install -d ${D}${prefix}/local/demo-ai/semantic-segmentation/tflite

    # install applications into the demo launcher
    install -m 0755 ${S}/tflite/400-tflite-semantic-segmentation-python.yaml	${D}${prefix}/local/demo/application

    # install application binaries and launcher scripts
    install -m 0755 ${S}/tflite/tflite_semantic_segmentation.py        ${D}${prefix}/local/demo-ai/semantic-segmentation/tflite
    install -m 0755 ${S}/tflite/launch_python*.sh		               ${D}${prefix}/local/demo-ai/semantic-segmentation/tflite
}

FILES:${PN} += "${prefix}/local/"

RDEPENDS:${PN} += " \
	python3-core \
	python3-numpy \
	python3-opencv \
	python3-pillow \
	python3-pygobject \
	python3-tensorflow-lite \
	tflite-models-deeplabv3 \
    application-resources \
	bash \
"
