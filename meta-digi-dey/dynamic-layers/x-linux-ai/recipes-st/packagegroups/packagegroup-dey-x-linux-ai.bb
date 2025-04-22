# Copyright (C) 2025, Digi International Inc.
SUMMARY = "X-LINUX-AI subset used for ConnectCore Demo"

PACKAGE_ARCH = "${MACHINE_ARCH}"

inherit packagegroup python3-dir

COMMON_PACKAGES = " \
    stai-mpu-tools \
    tim-vx-tools \
"

TFLITE_PACKAGES = " \
    stai-mpu-image-classification-cpp-tfl-npu \
    stai-mpu-image-classification-python-tfl-npu \
    stai-mpu-object-detection-cpp-tfl-npu \
    stai-mpu-object-detection-python-tfl-npu \
    tflite-vx-delegate-example \
"

ONNX_PACKAGES = " \
    onnxruntime-tools                        \
    stai-mpu-image-classification-cpp-ort-npu    \
    stai-mpu-image-classification-python-ort-npu \
    stai-mpu-object-detection-python-ort-npu     \
    stai-mpu-object-detection-cpp-ort-npu        \
"

OPENVX_PACKAGES = " \
    nbg-benchmark \
    stai-mpu-image-classification-cpp-ovx-npu \
    stai-mpu-image-classification-python-ovx-npu \
    stai-mpu-object-detection-cpp-ovx-npu \
    stai-mpu-object-detection-python-ovx-npu \
    stai-mpu-semantic-segmentation-python-ovx-npu \
    stai-mpu-pose-estimation-python-ovx-npu \
    stai-mpu-face-recognition-cpp-ovx-npu \
"

RDEPENDS:${PN} += " \
    ${COMMON_PACKAGES} \
    ${TFLITE_PACKAGES} \
    ${OPENVX_PACKAGES} \
"
