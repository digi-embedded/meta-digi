# Copyright (C) 2024, STMicroelectronics - All Rights Reserved
SUMMARY = "STM32 ISP IQTune Linux application."
DESCRIPTION = "The STM32 IQTune tool is connected through USB to this \
               application and allow customer properly configure the DCMIPP ISP \
               with their own raw sensor tuning parameters."

LICENSE = "SLA0044"
LIC_FILES_CHKSUM  = "file://stm32-isp-iqtune-application/LICENSE;md5=91fc08c2e8dfcd4229b69819ef52827c"
NO_GENERIC_LICENSE[SLA0044] = "stm32-isp-iqtune-application/LICENSE"
LICENSE:${PN} = "SLA0044"

inherit python3-dir

DEPENDS += " linux-stm32mp gstreamer1.0-plugins-bad"

SRC_URI  = " file://stm32-isp-iqtune-application;subdir=sources "
SRC_URI += " file://resources;subdir=sources "

S = "${WORKDIR}/sources"

do_configure[noexec] = "1"
do_compile[noexec] = "1"

do_install() {
    install -d ${D}${prefix}/local/demo/gtk-application
    install -d ${D}${prefix}/local/x-linux-isp
    install -d ${D}${prefix}/local/x-linux-isp/stm32-isp-iqtune-app
    install -d ${D}${prefix}/local/x-linux-isp/stm32-isp-iqtune-app/resources

    # install applications into the demo launcher
    install -m 0755 ${S}/stm32-isp-iqtune-application/400-stm32-isp-iqtune-python.yaml ${D}${prefix}/local/demo/gtk-application

    # install application and launcher scripts
    install -m 0755 ${S}/stm32-isp-iqtune-application/stm32_isp_iqtune_app.py ${D}${prefix}/local/x-linux-isp/stm32-isp-iqtune-app
    install -m 0755 ${S}/stm32-isp-iqtune-application/stm32_isp_iqtune_com.py ${D}${prefix}/local/x-linux-isp/stm32-isp-iqtune-app
    install -m 0755 ${S}/stm32-isp-iqtune-application/launch_python*.sh ${D}${prefix}/local/x-linux-isp/stm32-isp-iqtune-app

    # install the LICENSE file associated with the scripts
    install -m 0444 ${S}/stm32-isp-iqtune-application/LICENSE ${D}${prefix}/local/x-linux-isp/stm32-isp-iqtune-app

    # install all resource files
    # .png files
    install -m 0644 ${S}/resources/*.png ${D}${prefix}/local/x-linux-isp/stm32-isp-iqtune-app/resources
    # configuration scripts
    install -m 0644 ${S}/resources/Default.css ${D}${prefix}/local/x-linux-isp/stm32-isp-iqtune-app/resources
}

inherit module-base

FILES:${PN} += "${prefix}/local/"

RDEPENDS:${PN} += " \
    gstreamer1.0-plugins-bad-waylandsink \
    gstreamer1.0-plugins-bad-debugutilsbad \
    gstreamer1.0-plugins-base-app \
    gstreamer1.0-plugins-base-videoconvertscale \
    gtk+3 \
    libcamera-stm32mp-gst (>=${LIBCAMERA_VERSION_MIN}) \
    usbotg-gadget-acm-config \
    usbotg-gadget-acm-uvc-config \
    ${PYTHON_PN}-core \
    ${PYTHON_PN}-pyserial \
    bash \
"
