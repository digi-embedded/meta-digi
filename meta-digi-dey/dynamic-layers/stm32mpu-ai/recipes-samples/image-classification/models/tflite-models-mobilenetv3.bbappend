# Copyright (C) 2024, Digi International Inc.

SRC_URI:remove = "https://tfhub.dev/iree/lite-model/mobilenet_v3_large_100_224/uint8/1?lite-format=tflite;subdir=mobilenet_v3_large_100_224_quant;name=mobilenet_v3_large_100_224_quant"
SRC_URI:append = "file://mobilenet_v3_large_100_224_quant.tflite"

do_install() {
    install -d ${D}${prefix}/local/demo-ai/image-classification/models/mobilenet
    install -d ${D}${prefix}/local/demo-ai/image-classification/models/mobilenet/testdata

    # install mobilenetV3 model + corresponding label file
    install -m 0644 ${S}/label*.txt                                       ${D}${prefix}/local/demo-ai/image-classification/models/mobilenet/labels_mobilenet_v3.txt
    install -m 0644 ${WORKDIR}/mobilenet_v3_large_100_224_quant.tflite    ${D}${prefix}/local/demo-ai/image-classification/models/mobilenet/
}
