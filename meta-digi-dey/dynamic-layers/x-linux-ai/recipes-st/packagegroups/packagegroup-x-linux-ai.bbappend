# Copyright (C) 2023, Digi International Inc.

RDEPENDS:packagegroup-x-linux-ai-tflite:remove:ccmp13 = " \
    tflite-cv-apps-image-classification-c++ \
    tflite-cv-apps-object-detection-c++ \
"

RDEPENDS:packagegroup-x-linux-ai-tflite-edgetpu:remove:ccmp13 = " \
    tflite-cv-apps-edgetpu-image-classification-c++ \
    tflite-cv-apps-edgetpu-object-detection-c++ \
"

PACKAGES:remove:ccmp25 = " \
    packagegroup-x-linux-ai-tflite-edgetpu   \
    packagegroup-x-linux-ai-onnxruntime      \
"

RDEPENDS:packagegroup-x-linux-ai:remove:ccmp25 = " \
    packagegroup-x-linux-ai-tflite-edgetpu   \
    packagegroup-x-linux-ai-onnxruntime      \
"

RDEPENDS:packagegroup-x-linux-ai-tflite:remove:ccmp25 = " \
    tflite-cv-apps-image-classification-c++ \
    tflite-cv-apps-image-classification-python \
    tflite-cv-apps-object-detection-c++ \
    tflite-cv-apps-object-detection-python \
"

RDEPENDS:packagegroup-x-linux-ai-tflite:append:ccmp25 = " \
    tflite-image-classification-python \
    tflite-object-detection-python \
    tflite-pose-estimation-python \
    tflite-semantic-segmentation-python \
    tim-vx \
"
