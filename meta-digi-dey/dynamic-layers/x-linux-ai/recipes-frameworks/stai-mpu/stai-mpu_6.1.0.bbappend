# Copyright (C) 2025, Digi International Inc.

# Even if you choose not to install some of these packages, having them present
# in the recipe causes bitbake to always generate them, which pulls in a lot of
# spurious build-time dependencies. Since these dependencies are already in the
# higher level packagegroup recipes, remove them.
RDEPENDS:${PN}-tflite:remove = " \
    ${PYTHON_PN}-tensorflow-lite \
    tensorflow-lite \
    tflite-vx-delegate \
"
RDEPENDS:${PN}-ort:remove = " \
    onnxruntime \
    ${PYTHON_PN}-onnxruntime \
"

# Removing ort/tflite runtime dependencies causes package QA errors because
# they contain prebuilt libraries that link to onnxruntime/tensorflow-lite.
# Skip the specific QA check causing the errors to be able to generate all
# packages regardless of whether you end up installing them or not.
INSANE_SKIP:${PN}-tflite:append = " file-rdeps"
INSANE_SKIP:${PN}-ort:append = " file-rdeps"
