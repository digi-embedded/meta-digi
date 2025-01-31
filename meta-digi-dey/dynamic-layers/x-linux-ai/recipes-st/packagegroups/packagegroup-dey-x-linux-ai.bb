# Copyright (C) 2025, Digi International Inc.
SUMMARY = "X-LINUX-AI subset used for ConnectCore Demo"

PACKAGE_ARCH = "${MACHINE_ARCH}"

inherit packagegroup python3-dir

COMMON_PACKAGES = " \
    stai-mpu-tools \
    tim-vx-tools \
"

TFLITE_PACKAGES = " \
    stai-mpu-image-classification-cpp-tfl \
    stai-mpu-image-classification-python-tfl \
    stai-mpu-object-detection-cpp-tfl \
    stai-mpu-object-detection-python-tfl \
    tflite-vx-delegate-example \
"

ONNX_PACKAGES = " \
    onnxruntime-tools                        \
    stai-mpu-image-classification-cpp-ort    \
    stai-mpu-image-classification-python-ort \
    stai-mpu-object-detection-python-ort     \
    stai-mpu-object-detection-cpp-ort        \
"

OPENVX_PACKAGES = " \
    nbg-benchmark \
    stai-mpu-image-classification-cpp-ovx \
    stai-mpu-image-classification-python-ovx \
    stai-mpu-object-detection-cpp-ovx \
    stai-mpu-object-detection-python-ovx \
    stai-mpu-semantic-segmentation-python-ovx \
    stai-mpu-pose-estimation-python-ovx \
    stai-mpu-face-recognition-cpp-ovx \
"

RDEPENDS:${PN} += " \
    ${COMMON_PACKAGES} \
    ${TFLITE_PACKAGES} \
    ${OPENVX_PACKAGES} \
"
