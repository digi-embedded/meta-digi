# Copyright (C) 2023, STMicroelectronics - All Rights Reserved
SUMMARY = "NBG C++ Computer Vision image classification application example"
LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/BSD-3-Clause;md5=550794465ba0ec5312d6919e203a55f9"

inherit pkgconfig

COMPATIBLE_MACHINE = "stm32mp25common"

DEPENDS += "jpeg gcnano-driver-stm32mp gcnano-userland opencv gstreamer1.0-plugins-base gstreamer1.0-plugins-bad "

SRC_URI  =  " file://nbg;subdir=${BPN}-${PV} "

S = "${WORKDIR}/${BPN}-${PV}"

do_configure[noexec] = "1"

EXTRA_OEMAKE  = 'SYSROOT="${RECIPE_SYSROOT}"'

do_compile() {
    #Check the version of OpenCV and fill OPENCV_VERSION accordingly
    FILE=${RECIPE_SYSROOT}/${libdir}/pkgconfig/opencv4.pc
    if [ -f "$FILE" ]; then
        OPENCV_VERSION=opencv4
    else
        OPENCV_VERSION=opencv
    fi

    oe_runmake OPENCV_PKGCONFIG=${OPENCV_VERSION} NEW_GST_WAYLAND_API=${NEW_GST_WAYLAND_API} -C ${S}/nbg/
}

do_install() {
    install -d ${D}${prefix}/local/demo/application
    install -d ${D}${prefix}/local/demo-ai/image-classification/nbg

    # install applications into the demo launcher
    install -m 0755 ${S}/nbg/130-nbg-image-classification-C++.yaml	${D}${prefix}/local/demo/application

    # install application binaries and launcher scripts
    install -m 0755 ${S}/nbg/nbg_image_classification          ${D}${prefix}/local/demo-ai/image-classification/nbg
    install -m 0755 ${S}/nbg/launch_bin*.sh		               ${D}${prefix}/local/demo-ai/image-classification/nbg
}

FILES:${PN} += "${prefix}/local/"

INSANE_SKIP:${PN} = "ldflags"

RDEPENDS:${PN} += " \
	gstreamer1.0-plugins-bad-waylandsink \
	gstreamer1.0-plugins-bad-debugutilsbad \
	gstreamer1.0-plugins-base-app \
	gstreamer1.0-plugins-base-videorate \
	gstreamer1.0-plugins-good-video4linux2 \
	gtk+3 \
	libopencv-core \
	libopencv-imgproc \
	libopencv-imgcodecs \
    application-resources \
    nbg-models-mobilenetv3 \
	bash \
"

#Depending of the Gstreamer version supported by the Yocto version the RDEPENDS differs
RDEPENDS:${PN} += "${@bb.utils.contains('DISTRO_CODENAME', 'kirkstone', ' gstreamer1.0-plugins-base-videoscale gstreamer1.0-plugins-base-videoconvert ', ' gstreamer1.0-plugins-base-videoconvertscale ',  d)}"
