# Copyright (C) 2020, STMicroelectronics - All Rights Reserved
SUMMARY = "TensorFlowLite C++ API Computer Vision object detection application example running on the EdgeTPU"
LICENSE = "BSD-3-Clause & Apache-2.0"
LIC_FILES_CHKSUM  = "file://${COMMON_LICENSE_DIR}/BSD-3-Clause;md5=550794465ba0ec5312d6919e203a55f9"
LIC_FILES_CHKSUM += "file://${COMMON_LICENSE_DIR}/Apache-2.0;md5=89aea4e17d99a7cacdbeed46a0096b10"

inherit pkgconfig

DEPENDS += "tensorflow-lite libedgetpu gtk+3 opencv gstreamer1.0 rapidjson gstreamer1.0-plugins-base gstreamer1.0-plugins-bad "

SRC_URI  =  " file://tflite;subdir=${BPN}-${PV} "

S = "${WORKDIR}/${BPN}-${PV}"

do_configure[noexec] = "1"

EXTRA_OEMAKE   = 'SYSROOT="${RECIPE_SYSROOT}"'
EXTRA_OEMAKE  += 'EDGETPU=TRUE'

do_compile() {
    #Check the version of OpenCV and fill OPENCV_VERSION accordingly
    FILE=${RECIPE_SYSROOT}/${libdir}/pkgconfig/opencv4.pc
    if [ -f "$FILE" ]; then
        OPENCV_VERSION=opencv4
    else
        OPENCV_VERSION=opencv
    fi

    oe_runmake OPENCV_PKGCONFIG=${OPENCV_VERSION} -C ${S}/tflite/
}

do_install() {
    install -d ${D}${prefix}/local/demo/application
    install -d ${D}${prefix}/local/demo-ai/object-detection/coral

    # install applications into the demo launcher
    install -m 0755 ${S}/tflite/211-coral-object-detection-C++.yaml	${D}${prefix}/local/demo/application

    # install application binaries and launcher scripts
    install -m 0755 ${S}/tflite/tflite_object_detection        ${D}${prefix}/local/demo-ai/object-detection/coral/coral_object_detection
    install -m 0755 ${S}/tflite/launch_coral_bin*.sh		   ${D}${prefix}/local/demo-ai/object-detection/coral
}

FILES:${PN} += "${prefix}/local/"

INSANE_SKIP:${PN} = "ldflags"

RDEPENDS:${PN} += " \
	gstreamer1.0-plugins-bad-waylandsink \
	gstreamer1.0-plugins-bad-debugutilsbad \
	gstreamer1.0-plugins-base-app \
	gstreamer1.0-plugins-base-videorate \
	gstreamer1.0-plugins-good-video4linux2 \
	gstreamer1.0-plugins-base-videoconvertscale \
	gtk+3 \
	libedgetpu \
	libopencv-core \
	libopencv-imgproc \
	libopencv-imgcodecs \
	coral-models-coco-ssd-mobilenetv1 \
    application-resources \
	bash \
"
