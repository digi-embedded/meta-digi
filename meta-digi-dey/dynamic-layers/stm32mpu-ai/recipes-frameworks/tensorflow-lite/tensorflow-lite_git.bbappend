# Copyright (C) 2024, Digi International Inc.

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI += " file://0006-TFLite-cmake-add-XNNPACK-delegate-u8-and-i8-definition.patch "
SRC_URI += " file://0008-TFLite-cmake-change-visibility-compilation-options.patch "
SRC_URI:append:stm32mp2common = " file://0007-TFLite-fix-aarch64-support-for-XNNPACK.patch "

# Set building environment variables
TENSORFLOW_TARGET:aarch64="linux"
TENSORFLOW_TARGET_ARCH:aarch64="aarch64"

# Activate -O3 optimization and disable debug symbols
OECMAKE_C_FLAGS_RELEASE = "-O3 -DNDEBUG"
OECMAKE_CXX_FLAGS_RELEASE = "-O3 -DNDEBUG"
# Build tensorflow-lite.so library, _pywrap_tensorflow_interpreter_wrapper.so library and the benchmark_model application
OECMAKE_TARGET_COMPILE =  "tensorflow-lite _pywrap_tensorflow_interpreter_wrapper benchmark_model"

EXTRA_OECMAKE += " -DTFLITE_ENABLE_XNNPACK=OFF "

do_compile() {
    # Standard CMAKE build.
    cmake_runcmake_build --target ${OECMAKE_TARGET_COMPILE}
	# Build the python wheel (procedure extract form the build_pip_package_with_cmake.sh)
	BUILD_DIR=${WORKDIR}/build
	TENSORFLOW_DIR=${S}
	TENSORFLOW_LITE_DIR="${TENSORFLOW_DIR}/tensorflow/lite"
	TENSORFLOW_VERSION=$(grep "_VERSION = " "${TENSORFLOW_DIR}/tensorflow/tools/pip_package/setup.py" | cut -d= -f2 | sed "s/[ '-]//g")
	mkdir -p "${BUILD_DIR}/tflite_runtime"
	cp -r "${TENSORFLOW_LITE_DIR}/tools/pip_package/debian" \
	      "${TENSORFLOW_LITE_DIR}/tools/pip_package/MANIFEST.in" \
	      "${BUILD_DIR}"
	cp -r "${TENSORFLOW_LITE_DIR}/python/interpreter_wrapper" "${BUILD_DIR}"
	cp "${TENSORFLOW_LITE_DIR}/tools/pip_package/setup_with_binary.py" "${BUILD_DIR}/setup.py"
	cp "${TENSORFLOW_LITE_DIR}/python/interpreter.py" \
	   "${TENSORFLOW_LITE_DIR}/python/metrics/metrics_interface.py" \
	   "${TENSORFLOW_LITE_DIR}/python/metrics/metrics_portable.py" \
	   "${BUILD_DIR}/tflite_runtime"
	echo "__version__ = '${TENSORFLOW_VERSION}'" >> "${BUILD_DIR}/tflite_runtime/__init__.py"
	echo "__git_version__ = '$(git -C "${TENSORFLOW_DIR}" describe)'" >> "${BUILD_DIR}/tflite_runtime/__init__.py"

	export PACKAGE_VERSION="${TENSORFLOW_VERSION}"
	export PROJECT_NAME="tflite_runtime"
	cp "${BUILD_DIR}/_pywrap_tensorflow_interpreter_wrapper.so" "tflite_runtime"

	setuptools3_do_compile
}

# Require the external NPU delegate.
RDEPENDS:${PN}:append:stm32mp25common = " tflite-vx-delegate "
