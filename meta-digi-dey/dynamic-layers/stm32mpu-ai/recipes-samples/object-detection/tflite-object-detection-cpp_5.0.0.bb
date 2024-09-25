# Copyright (C) 2020, STMicroelectronics - All Rights Reserved
SUMMARY = "TensorFlowLite C++ API Computer Vision object detection application example"
LICENSE = "BSD-3-Clause & Apache-2.0"
LIC_FILES_CHKSUM  = "file://${COMMON_LICENSE_DIR}/BSD-3-Clause;md5=550794465ba0ec5312d6919e203a55f9"
LIC_FILES_CHKSUM += "file://${COMMON_LICENSE_DIR}/Apache-2.0;md5=89aea4e17d99a7cacdbeed46a0096b10"

inherit pkgconfig

DEPENDS += "tensorflow-lite gtk+3 opencv gstreamer1.0 rapidjson gstreamer1.0-plugins-base gstreamer1.0-plugins-bad"
DEPENDS:append:stm32mp25common = " tflite-vx-delegate "

SRC_URI  =  " file://tflite;subdir=${BPN}-${PV} "

S = "${WORKDIR}/${BPN}-${PV}"

do_configure[noexec] = "1"

BOARD_USED:stm32mp1common = "stm32mp1"
BOARD_USED:stm32mp25common = "stm32mp2_npu"

EXTRA_OEMAKE  = 'SYSROOT="${RECIPE_SYSROOT}"'
EXTRA_OEMAKE += 'ARCHITECTURE="${BOARD_USED}"'

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
    install -d ${D}${prefix}/local/demo-ai/object-detection/tflite

    # install applications into the demo launcher
    install -m 0755 ${S}/tflite/201-tflite-object-detection-C++.yaml	${D}${prefix}/local/demo/application

    # install application binaries and launcher scripts
    install -m 0755 ${S}/tflite/tflite_object_detection        ${D}${prefix}/local/demo-ai/object-detection/tflite
    install -m 0755 ${S}/tflite/launch_bin*.sh		           ${D}${prefix}/local/demo-ai/object-detection/tflite
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
	libopencv-core \
	libopencv-imgproc \
	libopencv-imgcodecs \
	tensorflow-lite \
	tflite-models-coco-ssd-mobilenetv1 \
    application-resources \
	rapidjson \
	bash \
"

#Depending of the Gstreamer version supported by the Yocto version the RDEPENDS differs
RDEPENDS:${PN} += "${@bb.utils.contains('DISTRO_CODENAME', 'kirkstone', ' gstreamer1.0-plugins-base-videoscale gstreamer1.0-plugins-base-videoconvert ', ' gstreamer1.0-plugins-base-videoconvertscale ',  d)}"
