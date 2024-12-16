# Copyright (C) 2023,2024 Digi International Inc.

RDEPENDS:packagegroup-x-linux-ai-tflite:remove:ccmp13 = " \
    tflite-cv-apps-image-classification-c++ \
    tflite-cv-apps-object-detection-c++ \
"

RDEPENDS:packagegroup-x-linux-ai-tflite-edgetpu:remove:ccmp13 = " \
    tflite-cv-apps-edgetpu-image-classification-c++ \
    tflite-cv-apps-edgetpu-object-detection-c++ \
"

RDEPENDS:packagegroup-x-linux-ai:remove:ccmp25 = " \
    packagegroup-x-linux-ai-onnxruntime      \
"

RDEPENDS:packagegroup-x-linux-ai-tflite:remove:ccmp25 = " \
    x-linux-ai-tool                          \
    x-linux-ai-application                   \
"

RDEPENDS:packagegroup-x-linux-ai-onnxruntime:remove:ccmp25 = " \
    x-linux-ai-tool                          \
    x-linux-ai-application                   \
"

RDEPENDS:packagegroup-x-linux-ai-npu:remove:ccmp25 = " \
    x-linux-ai-tool                          \
    x-linux-ai-application                   \
    ort-vsinpu-ep-example-cpp                \
    ort-vsinpu-ep-example-python             \
"
