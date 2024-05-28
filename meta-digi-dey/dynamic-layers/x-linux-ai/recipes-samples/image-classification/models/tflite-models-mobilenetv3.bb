# Copyright (C) 2019, STMicroelectronics - All Rights Reserved
SUMMARY = "Create package containing MobileNetV3 model"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/Apache-2.0;md5=89aea4e17d99a7cacdbeed46a0096b10"

SRC_URI  =  " https://tfhub.dev/iree/lite-model/mobilenet_v3_large_100_224/uint8/1?lite-format=tflite;subdir=mobilenet_v3_large_100_224_quant;name=mobilenet_v3_large_100_224_quant "
SRC_URI  += " file://labels_mobilenet_tflite.txt "

SRC_URI[mobilenet_v3_large_100_224_quant.md5sum] = "68451f4fdc681bca5c548c595386b918"
SRC_URI[mobilenet_v3_large_100_224_quant.sha256sum] = "78c5eddce8533ca95b4d122ec291694263b91c285616a9487f470c6a930a63cd"

S = "${WORKDIR}"

do_configure[noexec] = "1"
do_compile[noexec] = "1"

do_install() {
    install -d ${D}${prefix}/local/demo-ai/image-classification/models/mobilenet
    install -d ${D}${prefix}/local/demo-ai/image-classification/models/mobilenet/testdata

    #the model is fetched with a bad name -> rename it before installation
    mv ${S}/mobilenet_v3_large_100_224_quant/1\?lite-format\=tflite ${S}/mobilenet_v3_large_100_224_quant/mobilenet_v3_large_100_224_quant.tflite
    # install mobilenetV3 model + corresponding label file
    install -m 0644 ${S}/label*.txt                                     ${D}${prefix}/local/demo-ai/image-classification/models/mobilenet/labels_mobilenet_v3.txt
    install -m 0644 ${S}/mobilenet_v3_large_100_224_quant/*.tflite      ${D}${prefix}/local/demo-ai/image-classification/models/mobilenet/

}

FILES:${PN} += "${prefix}/local/"
